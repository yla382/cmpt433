#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095


int getPotReading() {
	FILE *file = fopen(A2D_FILE_VOLTAGE0, "r");
	if (file == NULL) {
		printf("Error: unable to open in_voltage0_raw file\n");
		exit(-1);
	}

	int potReading = 0;
	int inputNum = fscanf(file, "%d", &potReading);
	if(inputNum <= 0) {
		printf("Error: UInable to read values from the in_voltage0_raw file\n");
	}

	fclose(file);
	return potReading;
}

int main() {
	printf("A2D Example\n");

	// long seconds = 1;
	// long nanoseconds = 0;
	// struct timespec delayReq = {seconds, nanoseconds};
	
	// int currentVal = getPotReading();
	while(true) {
		// int newVal = getPotReading();
		int sum = 0;
		for(int i = 0; i < 100; i++) {
			int newVal = getPotReading();
			sum += newVal;
			// printf("Potential Meter value: %d\n", newVal);
		}
		printf("Average val: %d\n", sum / 100);
		// if (currentVal != newVal) {
		// 	printf("Potential Meter value: %d\n", currentVal);
		// 	printf("Potential Meter value: %d\n", newVal);
		// 	currentVal = newVal;
		// }
		printf("\n");
		// nanosleep(&delayReq, (struct timespec *) NULL);
	}

	// while (true) {
	// 	int reading = getPotReading();
	// 	double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
	// 	printf("Value %5d ==> %5.4fV\n", reading, voltage);
	// }

	return 0;
}