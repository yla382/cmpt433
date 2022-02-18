#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "sleep.h"
#include "light_sampler.h"

#define MAX_STR_LEN 1024
#define INITIAL_BUFFER_SIZE 1000
#define THREAD_NUM 6
#define EXPONENTIAL_WEIGHT 0.999
#define HYSTERESIS 0.03
#define DIP_RANGE 0.1

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t potCond = PTHREAD_COND_INITIALIZER;
static pthread_t tids[THREAD_NUM];

static bool continueProgram = true; //condition for thread functions to keep running
static int latestLightVoltageCount = 0; //number of samples read per second
static int lightDipCount = 0;
static int currentPotVal = -1;
static double lightLevelAverage = -1; //exponential avaerage of samples

static char previousArg1[MAX_STR_LEN]; //previous request arg1
static char previousArg2[MAX_STR_LEN]; //previous request arg2


/*
Thread function to display sample status in on terminal every second
input: *void
output: *void
*/
static void *displaySampleStatus(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	while(continueProgram) {
		//Critical Section
		pthread_mutex_lock(&mutex);
		{
			if(currentPotVal <= -1) { //Halt thread until Pot value is initialized
				pthread_cond_wait(&potCond, &mutex); 
			}

			printf("Samples/s = %d  Pot Value = %d  history size = %d  avg = %0.3f  dips = %d\n", 
				latestLightVoltageCount, 
				currentPotVal, 
				bufferPointer->currentSize, 
				(lightLevelAverage < 0) ? 0:lightLevelAverage, 
				lightDipCount
			);
			displayBuffer(*bufferPointer, 200); //prints every 200th samples
			latestLightVoltageCount = 0;
		}
		pthread_mutex_unlock(&mutex);
		sleepNow(1, 0); //Sleep for 1 second
	}
	return NULL;
}


/*
Thread function to update pot value every second
input: *void
output: *void
*/
static void *readPotVal(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;

	while(continueProgram) {
		//Critical section
		pthread_mutex_lock(&mutex);
		{
			currentPotVal = getPotReading(); //Read pot value
			//if pot value is zero set to 1. Otherwise, actually pot value
			int updatingPotVal = currentPotVal > 0 ? currentPotVal : 1;
			//Set buffer value according to the new pot value
			resizeBuffer(bufferPointer, updatingPotVal);
		}

		//Let readLightSensorVal() and displaySampleStatus() continue 
		//if they are on hold
		pthread_cond_signal(&potCond);
		pthread_mutex_unlock(&mutex);

		sleepNow(1, 0); //Sleep for 1 second
	}
	return NULL;
}


/*
Thread function to read new voltage value from light sensor every 1ms
and add to buffer. Also calculate exponetial average.
Input: *void
output: *void
*/
static void *readLightSensorVal(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	latestLightVoltageCount = 0;
	
	while(continueProgram) {
		//Critical section
		pthread_mutex_lock(&mutex);
		{	
			if(currentPotVal <= -1) { //If pot value has not initialized yet, halt the function
				pthread_cond_wait(&potCond, &mutex); 
			}

			//Get current light level voltage and insert into buffer
			double lightLevelVoltage = getLightLevelVoltage();
			insertNum(bufferPointer, lightLevelVoltage);

			//If average has not been calculated, but the average be the latest
			// light voltage to be the current average
			if(lightLevelAverage == -1) {
				lightLevelAverage = lightLevelVoltage;	
			} else { // Calculate expotential average
				lightLevelAverage = (lightLevelAverage * EXPONENTIAL_WEIGHT) + (lightLevelVoltage * (1 - EXPONENTIAL_WEIGHT));
			}
		}
		latestLightVoltageCount++;
		pthread_mutex_unlock(&mutex);
		sleepNow(0, 1000000); //sleep for 1ms
	}
	return NULL;
}


/*
Thread function to display the current dip count
input: *void
output: *void
*/
static void *displayLightDipsCount(void *buffer) {
	//Initialize display setting
	initializeDisplay();
	while(continueProgram) {
		int currentDipCount = lightDipCount;
		int stringLen = snprintf(NULL, 0, "%d", currentDipCount);
		char *dipCountStr = malloc(sizeof(char) * stringLen + 1);
		
		snprintf(dipCountStr, stringLen + 1, "%d", currentDipCount);
		
		//show current dip count to display
		displayScreen(dipCountStr);
		free(dipCountStr);
		dipCountStr = NULL;
	}
	//Turn off display
	closeDisplay();
	return NULL;
}


/*
Thread function to count number of dips every 0.1s by
analyzing the samples in the buffer
output: *void
input: *void
*/
static void *countLightDips(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	while(continueProgram) {
		lightDipCount = 0;
		pthread_mutex_lock(&mutex);
		{
			//if lightlevel average is not initialized, no need to count dips
			if(lightLevelAverage <= -1) {
				lightDipCount = 0;
			} else {
				int historySize = bufferPointer->currentSize;
				double currentLightLevelAverage = lightLevelAverage;
				//get copy of current samples in buffer
				double currentLightSamples[historySize];
				getArray(*bufferPointer, currentLightSamples);
				
				int newDipCounts = 0;
				bool canDip = true;
				for(int i = 0; i < historySize; i++) {
					if(canDip) {
						//dip is detedcted if the sample is less than the average and (average - sample) is greater than 0.1
						if((currentLightLevelAverage > currentLightSamples[i]) && currentLightLevelAverage - currentLightSamples[i] >= DIP_RANGE) {
							canDip = false;
							newDipCounts++;
							continue;
						}
					}

					// apply HYSTERESIS to check whether samples can be checked for
					//dips in the next iteration
					if(!canDip && currentLightSamples[i] > currentLightLevelAverage + HYSTERESIS) {
						canDip = true;
					}
				}
				lightDipCount = newDipCounts;
			}
		}
		pthread_mutex_unlock(&mutex);
		sleepNow(0, 100000000); //sleep for 0.1s
	}
	return NULL;
}


/*
Saves previous request made by clients
input: *char, *char
ouput: void
*/
static void saveRequest(char *arg1, char  *arg2) {
	//Clear previousArg1 and previousArg2
	memset(previousArg1, 0, MAX_STR_LEN);
	memset(previousArg2, 0, MAX_STR_LEN);

	//Only save previous argument if they are not null
	if(arg1 != NULL) {
		snprintf(previousArg1, MAX_STR_LEN - 1, "%s", arg1);
	}

	if(arg2 != NULL) {
		snprintf(previousArg2, MAX_STR_LEN - 1, "%s", arg2);
	}
}


/*
Helper function to check if the request is valid
input: *char, *char, *char, bool
output: bool
*/
static bool validateArguments(char *arg1, char *arg2, char* compStr, bool isArg2Null) {
	if(isArg2Null) {
		return strcmp(arg1, compStr) == 0 && arg2 == NULL;
	} else {
		return strcmp(arg1, compStr) == 0 && arg2 != NULL;
	}
}


/*
Thread function uses UDP socket to listen to port 12345 for request
and reponess
input: *void
output: *void
*/
static void *udpNetwork(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;

	//initialize udp connection
	openConnection();
	while(continueProgram) {
		//Get raw request data from the client in upper case
		char request[MAX_STR_LEN];
		int requestLen = receiveRequest(request, MAX_STR_LEN);
		stringUpper(request, requestLen);

		//extract first two arguments from the request string
		char *arg1 = NULL;
		char *arg2 = NULL;
		extractRequest(request, &arg1, &arg2);

		//when request is blank, use previous request
		if(arg1 == NULL) {
			//only get previous request if previousArg1 contains valid string
			if(strlen(previousArg1) > 0) {
				arg1 = previousArg1;
				arg2 = strlen(previousArg2) > 0 ? previousArg2 : NULL;
			}
		} else {
			//Save the request
			saveRequest(arg1, arg2);
		}

		char *response = NULL;
		int arrSize = bufferPointer->currentSize;
		double currentArr[arrSize];

		if(arg1 == NULL) {
			sendResponse("No previous request found.\n", false);

		} else if(validateArguments(arg1, arg2, "HELP", true)) {
			getHelpString(&response);
			sendResponse(response, true);

		} else if(validateArguments(arg1, arg2, "COUNT", true)) {
			getCountString(&response, bufferPointer->historicCount);
			sendResponse(response, true);

		} else if(validateArguments(arg1, arg2, "LENGTH", true)) {
			getLengthString(&response, bufferPointer->size, bufferPointer->currentSize);
			sendResponse(response, true);

		} else if(validateArguments(arg1, arg2, "HISTORY", true)) {
			getArray(*bufferPointer, currentArr); //get copy of current buffer
			getHistoryString(&response, currentArr, arrSize, arrSize);
			sendResponse(response, true);

		} else if(validateArguments(arg1, arg2, "GET", false)) {
			int getSize = atoi(arg2);
			// when 2nd argument has invalid value send error message
			if (getSize <= 0 || getSize > arrSize) {
				sendResponse("Error: Invalid Request\n", false);
			} else {
				getArray(*bufferPointer, currentArr); //get copy of current buffer
				getHistoryString(&response, currentArr, arrSize, getSize);
				sendResponse(response, true);
			}
		} else if(validateArguments(arg1, arg2, "DIPS", true)) {
			getDipString(&response, lightDipCount);
			sendResponse(response, true);

		} else if(validateArguments(arg1, arg2, "STOP", true)) {
			sendResponse("Program terminating.\n", false);
			//set control varibale to true to make all thread functions
			//to exit out of their loop and complete the function
			continueProgram = false;
		} else {
			sendResponse("Error: Invalid Request.\n", false);
		}

		//clear request array
		memset(request, 0, MAX_STR_LEN);
		response = NULL;
	}

	//close udp connection
	closeConnection();
	return NULL;
}


/*
Samples light level voltage
Features:
	 - samples light level voltage
	 - read value from potentiometer to update sample history size
	 - calculate exponential average
	 - detect dips in light sensor
	 - display number of dips in 14 seg display
	 - UDP connection on port 12345 for remote query on samples
	 	- commands: help, count, length, get N, history, dips, stop
	 - uses 6 threads
input: void
output: void
*/
void light_sampler() {
	//Initialized buffer with default size
	CircularBuffer buffer = initializeBuffer(INITIAL_BUFFER_SIZE);

	//Create threads
	pthread_create(&tids[0], NULL, displaySampleStatus, &buffer);
	pthread_create(&tids[1], NULL, readPotVal, &buffer);
	pthread_create(&tids[2], NULL, readLightSensorVal, &buffer);
	pthread_create(&tids[3], NULL, displayLightDipsCount, &buffer);
	pthread_create(&tids[4], NULL, countLightDips, &buffer);
	pthread_create(&tids[5], NULL, udpNetwork, &buffer);

	//Wait until threads are done and clean up memories used by threads
	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tids[i], NULL);
	}

	//Free deallocated arrray in the buffer
	free(buffer.array);
}