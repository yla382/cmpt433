#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define DA_TRIGGER_USR_0 "/sys/class/leds/beaglebone:green:usr0/trigger"
#define DA_BIGHTNESS_USR_0 "/sys/class/leds/beaglebone:green:usr0/brightness"

void led_zero_trigger(char setting[]) {

	FILE *pLedTriggerFile = fopen(DA_TRIGGER_USR_0, "w");

	if(pLedTriggerFile == NULL) {
		printf("ERROR Opening %s. ", DA_TRIGGER_USR_0);
		exit(1);
	}

	int charWritten = fprintf(pLedTriggerFile, "%s", setting);
	if(charWritten <= 0) {
		printf("ERROR WRITING DATA");
		exit(1);
	}

	fclose(pLedTriggerFile);
}


void led_zero_brightness(bool led_on) {
	FILE *pLedBrightnessFile = fopen(DA_BIGHTNESS_USR_0, "w");

	if(pLedBrightnessFile == NULL) {
		printf("Error Opening %s ", DA_BIGHTNESS_USR_0);
		exit(1);
	}

	int charWritten = -1;

	if(led_on == true) {
		charWritten = fprintf(pLedBrightnessFile, "1");
	} else {
		charWritten = fprintf(pLedBrightnessFile, "0");
	}

	if(charWritten <= 0) {
		printf("ERROR WRITING DATA");
		exit(1);
	}

	fclose(pLedBrightnessFile);
}


int main() {
	led_zero_trigger("none");

	long seconds = 1;
	long nanoseconds = 500000000;
	struct timespec reqDelay = {seconds, nanoseconds};

	led_zero_brightness(true);
	nanosleep(&reqDelay, (struct timespec *) NULL);
	led_zero_brightness(false);
	nanosleep(&reqDelay, (struct timespec *) NULL);
	led_zero_brightness(true);
	nanosleep(&reqDelay, (struct timespec *) NULL);
	led_zero_brightness(false);

	return 0;
}