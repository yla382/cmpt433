#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "sleep.h"
#include "circular_buffer.h"
#include "a2d.h"
#include "display.h"

#define INITIAL_BUFFER_SIZE 1000
#define THREAD_NUM 5
#define EXPONENTIAL_WEIGHT 0.999

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t potCond = PTHREAD_COND_INITIALIZER;
static pthread_t tids[THREAD_NUM];
static int latestLightVoltageCount = 0;
static int lightDipCount = 0;
static int currentPotVal = -1;
static double lightLevelAverage = -1;


void *displaySampleStatus(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	//sleepNow(0, 50000000);
	while(true) {
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
}

void *readPotVal(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	
	while(true) {
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
}

void *readLightSensorVal(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	latestLightVoltageCount = 0;
	
	while(true) {
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
}


void *displayLightDipsCount(void *buffer) {
	while(true) {
		int currentDipCount = lightDipCount;
		int stringLen = snprintf(NULL, 0, "%d", currentDipCount);
		char *dipCountStr = malloc(sizeof(char) * stringLen + 1);
		
		snprintf(dipCountStr, stringLen + 1, "%d", currentDipCount);
		
		displayScreen(dipCountStr);
		free(dipCountStr);
		dipCountStr = NULL;
	}
}

void *countLightDips(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	while(true) {
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
						if((currentLightLevelAverage > currentLightSamples[i]) && currentLightLevelAverage - currentLightSamples[i] >= 0.1) {
							canDip = false;
							newDipCounts++;
							continue;
						}
					}

					if(!canDip && currentLightSamples[i] > currentLightLevelAverage + 0.03) {
						canDip = true;
					}
				}
				lightDipCount = newDipCounts;
			}
		}
		pthread_mutex_unlock(&mutex);
		sleepNow(0, 100000000);
	}
}

int main() {
	initializeDisplay();
	CircularBuffer buffer = initializeBuffer(INITIAL_BUFFER_SIZE);

	pthread_create(&tids[0], NULL, displaySampleStatus, &buffer);
	pthread_create(&tids[1], NULL, readPotVal, &buffer);
	pthread_create(&tids[2], NULL, readLightSensorVal, &buffer);
	pthread_create(&tids[3], NULL, displayLightDipsCount, &buffer);
	pthread_create(&tids[4], NULL, countLightDips, &buffer);

	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tids[i], NULL);
	}

	free(buffer.array);
	closeDisplay();
	return 0;
}