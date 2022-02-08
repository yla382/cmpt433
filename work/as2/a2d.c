#include "a2d.h"

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define LOOP_NUM 100

static int getPotReading() {
	FILE *file = fopen(A2D_FILE_VOLTAGE0, "r");
	if (file == NULL) {
		printf("Error: unable to open in_voltage0_raw file\n");
		exit(-1);
	}

	int potReading = 0;
	int inputNum = fscanf(file, "%d", &potReading);
	if(inputNum <= 0) {
		printf("Error: UInable to read values from the in_voltage0_raw file\n");
		exit(-1);
	}

	fclose(file);
	return potReading;
}

int getStablePotReading() {
	int sumVal = 0;
	for(int i = 0; i < LOOP_NUM; i++) {
		sumVal += getPotReading();
	}

	int averageVal = sumVal / LOOP_NUM;
	return averageVal;
}

static int getLightLevelReading() {
	FILE *file = fopen(A2D_FILE_VOLTAGE1, "r");
	if (file == NULL) {
		printf("Error: unable to open in_voltage1_raw file\n");
		exit(-1);
	}

	int lightLevelReading = 0;
	int inputNum = fscanf(file, "%d", &lightLevelReading);
	if(inputNum <= 0) {
		printf("Error: UInable to read values from the in_voltage1_raw file\n");
		exit(-1);
	}

	fclose(file);
	return lightLevelReading;
}

double getLightLevelVoltage() {
	return ((double) getLightLevelReading() / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
}