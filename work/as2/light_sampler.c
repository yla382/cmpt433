#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "sleep.h"
#include "circular_buffer.h"
#include "a2d.h"
#include "display.h"

#define INITIAL_BUFFER_SIZE 1000
#define THREAD_NUM 4

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t potCond = PTHREAD_COND_INITIALIZER;
static pthread_t tids[THREAD_NUM];
static int latestLightVoltageCount = 0;
static int currentPotVal = -1;

void *displaySampleStatus(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;

	while(true) {
		pthread_mutex_lock(&mutex);
		{
			if(currentPotVal <= -1) {
				pthread_cond_wait(&potCond, &mutex); 
			}
			printf("Samples/s = %d  Pot Value = %d  history size = %d  max size = %d  dips = %d\n", 
				latestLightVoltageCount, 
				currentPotVal, 
				bufferPointer->currentSize, 
				bufferPointer->size, 
				0
			);
			displayBuffer(*bufferPointer, 200);
			latestLightVoltageCount = 0;
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
		}
		latestLightVoltageCount++;
		pthread_mutex_unlock(&mutex);
	}
}


void *displayLightDipsCount(void *buffer) {
	while(true) {
		displayScreen("50");
	}
}


int main() {
	initializeDisplay();
	CircularBuffer buffer = initializeBuffer(INITIAL_BUFFER_SIZE);

	pthread_create(&tids[0], NULL, displaySampleStatus, &buffer);
	pthread_create(&tids[1], NULL, readPotVal, &buffer);
	pthread_create(&tids[2], NULL, readLightSensorVal, &buffer);
	pthread_create(&tids[3], NULL, displayLightDipsCount, &buffer);

	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tids[i], NULL);
	}

	free(buffer.array);
	closeDisplay();
	return 0;
}