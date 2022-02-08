#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include "circular_buffer.h"
#include "a2d.h"

#define INITIAL_BUFFER_SIZE 1000
#define THREAD_NUM 3

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t potCond = PTHREAD_COND_INITIALIZER;
static pthread_t tids[THREAD_NUM];
static int currentPotVal = -1;

void *displaySampleStatus(void *buffer) {
	long seconds = 1;
	long nanoseconds = 0;
	struct timespec delayReq = {seconds, nanoseconds};

	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	while(true) {
		pthread_mutex_lock(&mutex);
		{
			if(currentPotVal <= -1) {
				pthread_cond_wait(&potCond, &mutex); 
			}
			printf("Samples/s = %d  Pot Value = %d  history size = %d  avg = %d  dips = %d\n", 0, currentPotVal, bufferPointer->size, 0, 0);
			//displayBuffer(*bufferPointer);
		}
		pthread_mutex_unlock(&mutex);
		nanosleep(&delayReq, (struct timespec *) NULL);
	}
}

void *readPotVal(void *buffer) {
	long seconds = 1;
	long nanoseconds = 0;
	struct timespec delayReq = {seconds, nanoseconds};

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

		nanosleep(&delayReq, (struct timespec *) NULL);
	}
}

void *readLightSensorVal(void *buffer) {
	CircularBuffer *bufferPointer = (CircularBuffer *) buffer;
	
	while(true) {
		pthread_mutex_lock(&mutex);
		{	
			pthread_cond_wait(&potCond, &mutex);
			double lightLevelVoltage = getLightLevelVoltage();
			insertNum(bufferPointer, lightLevelVoltage);
		}
		pthread_mutex_unlock(&mutex);
	}
}


int main() {
	CircularBuffer buffer = initializeBuffer(INITIAL_BUFFER_SIZE);

	pthread_create(&tids[0], NULL, readPotVal, &buffer);
	pthread_create(&tids[1], NULL, displaySampleStatus, &buffer);
	pthread_create(&tids[2], NULL, readLightSensorVal, &buffer);

	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tids[i], NULL);
	}

	free(buffer.array);
	return 0;
}