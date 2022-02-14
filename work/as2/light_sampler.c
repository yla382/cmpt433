#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "sleep.h"
#include "circular_buffer.h"
#include "a2d.h"
#include "display.h"
#include "command_string.h"
#include "network.h"

#define MAX_STR_LEN 1024
#define INITIAL_BUFFER_SIZE 1000
#define THREAD_NUM 6
#define EXPONENTIAL_WEIGHT 0.999
#define HYSTERESIS 0.03
#define DIP_RANGE 0.1

static bool continueProgram = true;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t potCond = PTHREAD_COND_INITIALIZER;
static pthread_t tids[THREAD_NUM];
static int latestLightVoltageCount = 0;
static int lightDipCount = 0;
static int currentPotVal = -1;
static double lightLevelAverage = -1;

static char previousArg1[MAX_STR_LEN];
static char previousArg2[MAX_STR_LEN];


void *displaySampleStatus(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	//sleepNow(0, 50000000);
	while(continueProgram) {
		pthread_mutex_lock(&mutex);
		{
			if(currentPotVal <= -1) {
				pthread_cond_wait(&potCond, &mutex); 
			}


			printf("Samples/s = %d  Pot Value = %d  history size = %d  avg = %0.3f  dips = %d\n", 
				latestLightVoltageCount, 
				currentPotVal, 
				bufferPointer->currentSize, 
				(lightLevelAverage < 0) ? 0:lightLevelAverage, 
				lightDipCount
			);
			displayBuffer(*bufferPointer, 200);
			latestLightVoltageCount = 0;
			//lightLevelAverage = -1;
		}
		pthread_mutex_unlock(&mutex);
		sleepNow(1, 0);


	}
	pthread_exit(NULL);
}

void *readPotVal(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	
	while(continueProgram) {
		pthread_mutex_lock(&mutex);
		{
			currentPotVal = getStablePotReading();
			int updatingPotVal = currentPotVal > 0 ? currentPotVal : 1;
			resizeBuffer(bufferPointer, updatingPotVal);
		}

		pthread_cond_signal(&potCond);
		pthread_mutex_unlock(&mutex);

		sleepNow(1, 0);
	}
	pthread_exit(NULL);
}

void *readLightSensorVal(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	latestLightVoltageCount = 0;
	
	while(continueProgram) {
		pthread_mutex_lock(&mutex);
		{	
			if(currentPotVal <= -1) {
				pthread_cond_wait(&potCond, &mutex); 
			}

			double lightLevelVoltage = getLightLevelVoltage();
			insertNum(bufferPointer, lightLevelVoltage);

			if(lightLevelAverage == -1) {
				lightLevelAverage = lightLevelVoltage;	
			} else {
				lightLevelAverage = (lightLevelAverage * EXPONENTIAL_WEIGHT) + (lightLevelVoltage * (1 - EXPONENTIAL_WEIGHT));
			}
		}
		latestLightVoltageCount++;
		pthread_mutex_unlock(&mutex);
		sleepNow(0, 1000000);
	}
	pthread_exit(NULL);
}


void *displayLightDipsCount(void *buffer) {
	initializeDisplay();
	while(continueProgram) {
		int currentDipCount = lightDipCount;
		int stringLen = snprintf(NULL, 0, "%d", currentDipCount);
		char *dipCountStr = malloc(sizeof(char) * stringLen + 1);
		
		snprintf(dipCountStr, stringLen + 1, "%d", currentDipCount);
		
		displayScreen(dipCountStr);
		free(dipCountStr);
		dipCountStr = NULL;
	}
	closeDisplay();
	pthread_exit(NULL);
}

void *countLightDips(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	while(continueProgram) {
		lightDipCount = 0;
		pthread_mutex_lock(&mutex);
		{
			if(lightLevelAverage <= -1) {
				lightDipCount = 0;
			} else {
				int historySize = bufferPointer->currentSize;
				double currentLightLevelAverage = lightLevelAverage;
				double currentLightSamples[historySize];
				getArray(*bufferPointer, currentLightSamples);
				
				int newDipCounts = 0;
				bool canDip = true;
				for(int i = 0; i < historySize; i++) {
					if(canDip) {
						if((currentLightLevelAverage > currentLightSamples[i]) && currentLightLevelAverage - currentLightSamples[i] >= DIP_RANGE) {
							canDip = false;
							newDipCounts++;
							continue;
						}
					}

					if(!canDip && currentLightSamples[i] > currentLightLevelAverage + HYSTERESIS) {
						canDip = true;
					}
				}
				lightDipCount = newDipCounts;
			}
		}
		pthread_mutex_unlock(&mutex);
		sleepNow(0, 100000000);
	}
	pthread_exit(NULL);
}

void saveRequest(char *arg1, char  *arg2) {
	memset(previousArg1, 0, MAX_STR_LEN);
	memset(previousArg2, 0, MAX_STR_LEN);

	if(arg1 != NULL) {
		snprintf(previousArg1, MAX_STR_LEN - 1, "%s", arg1);
	}

	if(arg2 != NULL) {
		snprintf(previousArg2, MAX_STR_LEN - 1, "%s", arg2);
	}
}

void *udpNetwork(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	memset(previousArg1, 0, MAX_STR_LEN);
	memset(previousArg2, 0, MAX_STR_LEN);

	openConnection();
	while(continueProgram) {
		char request[MAX_STR_LEN];
		int requestLen = receiveRequest(request, MAX_STR_LEN);
		//request[requestLen - 2] = 0;
		stringUpper(request, requestLen);

		char *arg1 = NULL;
		char *arg2 = NULL;

		extractRequest(request, &arg1, &arg2);

		if(arg1 == NULL) {
			if(strlen(previousArg1) > 0) {
				arg1 = previousArg1;
				arg2 = previousArg2;
			}
		} else {
			saveRequest(arg1, arg2);
		}

		char *response = NULL;
		if(arg1 == NULL) {
			sendResponse("No previous request found.\n");

		} else if(strcmp(arg1, "HELP") == 0) {
			getHelpString(&response);
			sendResponse(response);
			free(response);

		} else if(strcmp(arg1, "COUNT") == 0) {
			getCountString(&response, bufferPointer->historicCount);
			sendResponse(response);
			free(response);

		} else if(strcmp(arg1, "LENGTH") == 0) {
			getLengthString(&response, bufferPointer->size, bufferPointer->currentSize);
			sendResponse(response);
			free(response);

		} else if(strcmp(arg1, "HISTORY") == 0) {
			int arrSize = bufferPointer->currentSize;
			double currentArr[arrSize];
			
			getArray(*bufferPointer, currentArr);
			getHistoryString(&response, currentArr, arrSize, arrSize);
			sendResponse(response);
			free(response);

		} else if(strcmp(arg1, "GET") == 0) {
			if(arg2 == NULL) {
				sendResponse("Error: Invalid Request\n");
			} else {
				int getSize = atoi(arg2);
				int arrSize = bufferPointer->currentSize;

				if (getSize <= 0 || getSize > arrSize) {
					sendResponse("Error: Invalid Request\n");
				} else {
					double currentArr[arrSize];
			
					getArray(*bufferPointer, currentArr);
					getHistoryString(&response, currentArr, arrSize, getSize);
					sendResponse(response);
					free(response);
				}

			}
		} else if(strcmp(arg1, "DIPS") == 0) {
			getDipString(&response, lightDipCount);
			sendResponse(response);
			free(response);

		} else if(strcmp(arg1, "STOP") == 0) {
			sendResponse("Program terminating.\n");
			continueProgram = false;
		} else {
			sendResponse("Invalid Request.\n");
		}

		memset(request, 0, MAX_STR_LEN);
		response = NULL;
	}
	closeConnection();
	pthread_exit(NULL);
}

int main() {
	CircularBuffer buffer = initializeBuffer(INITIAL_BUFFER_SIZE);

	pthread_create(&tids[0], NULL, displaySampleStatus, &buffer);
	pthread_create(&tids[1], NULL, readPotVal, &buffer);
	pthread_create(&tids[2], NULL, readLightSensorVal, &buffer);
	pthread_create(&tids[3], NULL, displayLightDipsCount, &buffer);
	pthread_create(&tids[4], NULL, countLightDips, &buffer);
	pthread_create(&tids[5], NULL, udpNetwork, &buffer);

	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tids[i], NULL);
	}

	free(buffer.array);
	return 0;
}