#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#define TRIGGER "/sys/class/leds/beaglebone:green:usr0/trigger"
#define BRIGHTNESS "/sys/class/leds/beaglebone:green:usr0/brightness"


int setTrigger(char* filename, char* triggerStr) {
	FILE *triggerFile = fopen(filename, "w");
	if(triggerFile == NULL) {
		return -1;
		printf("unable to access file\n");
		exit(1);
	}

	int fileWrite = fprintf(triggerFile, "%s", triggerStr);
	if(fileWrite <= 0 ) {
		printf("unable to write to file\n");
		return -1;
	}

	fclose(triggerFile);
	return 0;
}

int main() {
	setTrigger(TRIGGER, "none");
	return 0;
}