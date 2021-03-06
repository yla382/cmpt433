#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define UP "/sys/class/gpio/gpio26/value"
#define DOWN "/sys/class/gpio/gpio46/value"
#define RIGHT "/sys/class/gpio/gpio47/value"
#define LEFT "/sys/class/gpio/gpio65/value"

void exportGpioFiles(int gpioNum) {
	FILE *pFile = fopen("/sys/class/gpio/export", "w");
	if (pFile == NULL) {
		printf("ERROR: Unable to open export file.\n");
		exit(1);
	}

	fprintf(pFile, "%d", gpioNum);
	printf("GPIO #%d exported\n", gpioNum);
	fclose(pFile);

	long seconds = 0;
	long nanoseconds = 300000000;
	struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}

int getGpioVal(char *fileName) {
	FILE *pFile = fopen(fileName, "r");
	if (pFile == NULL) {
		printf("ERROR: Unable to open file (%s) for read\n", fileName);
		exit(-1);
	}
	// Read string (line)
	const int MAX_LENGTH = 1024;
	char buff[MAX_LENGTH];
	fgets(buff, MAX_LENGTH, pFile);
	// Close
	fclose(pFile);

	int resultVal = atoi(buff);
	return resultVal;
}

int main() {
	exportGpioFiles(26);
	exportGpioFiles(46);
	exportGpioFiles(47);
	exportGpioFiles(65);

	int upVal = 1;
	int downVal = 1;
	int leftVal = 1;
	int rightVal = 1;

	while(true) {
		upVal = getGpioVal(UP);
		downVal = getGpioVal(DOWN);
		leftVal = getGpioVal(LEFT);
		rightVal = getGpioVal(RIGHT);

		if(upVal == 0) {
			printf("UP detected\n");
		}

		if(downVal == 0) {
			printf("DOWN detected\n");
		}

		if(leftVal == 0) {
			printf("LEFT detected\n");
		}

		if(rightVal == 0) {
			printf("RIGHT detected\n");
		}

		upVal = 1;
		downVal = 1;
		leftVal = 1;
		rightVal = 1;

	}

	return 0;
}