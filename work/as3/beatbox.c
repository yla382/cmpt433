#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdbool.h>
#include <pthread.h>
#include "beatbox.h"
#include "audio.h"
#include "audioMixer_template.h"
#include "sleep.h"
#include "network.h"
#include "joyStickControl.h"
#include "accelerometer.h"

#define THREAD_NUM 5

static pthread_t tids[THREAD_NUM];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int switchVal = 1;
static int  mode = 2;
static Joy_Direction joyDirection = INVALID;


/*
Thread function to update current joystick input
input: void
output: *void
*/
static void *setJoyStickDirection()
{
	initializeJoyStick();
	while(switchVal) {
		joyDirection = getDirections();
	}

	printf("Thread: setJoyStickDirection complete\n");
	return NULL;
}


/*
Thread function to switch between beats when mode changes
input: void
output: *void
*/
static void *switchBeats() 
{
	initialize_audio_files();

	while(switchVal) {
		if(mode == 1) {
			continue;
		} else if(mode == 2) {
			standardRockBeat();
		} else {
			customDrumBeat();
		}
	}

	remove_audio_files();
	printf("Thread: switchBeats complete\n");
	return NULL;
}


/*
Thread function to change volum and tempo from joystick
input: void
output: *void
*/
static void *switchTempoAndVolumn() 
{
	while(switchVal) {
		// if(thread_controller) pthread_cond_wait(&mutex_cond, &mutex); 
		if(joyDirection == RIGHT) {
			pthread_mutex_lock(&mutex);
			{
				changeTempo(true);
			}
			pthread_mutex_unlock(&mutex);
		} else if (joyDirection == LEFT) {
			pthread_mutex_lock(&mutex);
			{
				changeTempo(false);
			}
			pthread_mutex_unlock(&mutex);
		} else if(joyDirection == UP){
			pthread_mutex_lock(&mutex);
			{
				changeVolumn(true);
			}
			pthread_mutex_unlock(&mutex);
		} else if(joyDirection == DOWN) {
			pthread_mutex_lock(&mutex);
			{
				changeVolumn(false);
			}
			pthread_mutex_unlock(&mutex);
		} else if(joyDirection == CENTER) {
			pthread_mutex_lock(&mutex);
			{
				mode = mode + 1 > 3 ? 1 : mode + 1;
			}
			pthread_mutex_unlock(&mutex);
		} else {
			continue;
		}
		sleepNow(0, 500000000);
	}
	printf("Thread: switchTempoAndVolumn complete\n");
	return NULL;
}


/*
Thread function to get value from accelerometer
input: void
output: *void
*/
static void *accelerometer() 
{
	Accelerometer_initialize();

	while(switchVal) {
		Accelerometer_sampler();
	}

	Accelerometer_stopSampling();
	printf("Thread: accelerometer complete\n");
	return NULL;
}



/*
Function to get string containing program info such as runtime
*/
static char *getProgramStatus() 
{
	int volumn = AudioMixer_getVolume();
	int tempo = getTempo();
	struct sysinfo info;
	sysinfo(&info);
	
	int hour, minute, seconds;
	hour = (info.uptime / 3600);
	minute = (info.uptime - (3600 * hour)) / 60;
	seconds = (info.uptime - (3600 * hour)- (minute * 60));
	
	static char status[40];
	memset(status, 0, sizeof(char)*40);
	snprintf(status, 40, "update,%d,%d,%d:%d:%d", volumn, tempo, hour, minute, seconds);
	return status;
}


/*
Thread function to communicate with nodeJS server to control audio
and send audio info to server
input: void
output: *void
*/
static void *udp() 
{
	openConnection();
	//Call mutex block
	while(switchVal) {
		char request[1024];
		receiveRequest(request, 1024);
		printf("%s\n", request);
		if(strcmp(request, "VOLUMN_UP") == 0) {
			pthread_mutex_lock(&mutex);
			{
				changeVolumn(true);
			}
			pthread_mutex_unlock(&mutex);
		} else if(strcmp(request, "VOLUMN_DOWN") == 0) {
			pthread_mutex_lock(&mutex);
			{
				changeVolumn(false);
			}
			pthread_mutex_unlock(&mutex);
		} else if(strcmp(request, "TEMPO_UP") == 0) {
			pthread_mutex_lock(&mutex);
			{
				changeTempo(true);
			}
			pthread_mutex_unlock(&mutex);
		} else if(strcmp(request, "TEMPO_DOWN") == 0) {
			pthread_mutex_lock(&mutex);
			{
				changeTempo(false);
			}
			pthread_mutex_unlock(&mutex);
		} else if(strcmp(request, "NONE") == 0){
			pthread_mutex_lock(&mutex);
			{
				mode = 1;
			}
			pthread_mutex_unlock(&mutex);
		} else if(strcmp(request, "ROCK_1") == 0) {
			pthread_mutex_lock(&mutex);
			{
				mode = 2;
			}
			pthread_mutex_unlock(&mutex);
		} else if(strcmp(request, "ROCK_2") == 0) {
			pthread_mutex_lock(&mutex);
			{
				mode = 3;
			}
			pthread_mutex_unlock(&mutex);
		} else if(strcmp(request, "HI_HAT") == 0) {
			queue_sound(DRUM_CYN_HARD);

		} else if(strcmp(request, "SNARE") == 0) {
			queue_sound(DRUM_SNARE_HARD);

		} else if(strcmp(request, "BASE") == 0) {
			queue_sound(DRUM_DB_HARD);

		} else if(strcmp(request, "UPDATE") == 0) {
			char *status = getProgramStatus();
			sendResponse(status);
		} else if(strcmp(request, "QUIT") == 0) {
			switchVal = false;
			break; //exit the while loop
		} else {
			continue;
		}
		memset(request, 0, sizeof(char)*1024);
	}


	closeConnection();

	printf("Thread: udp finished\n");
	return NULL;
}

void beatbox() 
{
	start_audio();
	Accelerometer_initialize();

	pthread_create(&tids[0], NULL, switchBeats, NULL);
	pthread_create(&tids[1], NULL, setJoyStickDirection, NULL);
	pthread_create(&tids[2], NULL, switchTempoAndVolumn, NULL);
	pthread_create(&tids[3], NULL, accelerometer, NULL);
	pthread_create(&tids[4], NULL, udp, NULL);

	//Wait until threads are done and clean up memories used by threads
	for(int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tids[i], NULL);
	}
}