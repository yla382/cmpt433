#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GPIO_30_DIRECTION "/sys/class/gpio/gpio30/direction"
#define GPIO_30_VALUE "/sys/class/gpio/gpio30/value"

int main() {
	printf("Initiating GPIO program\n");

	FILE *gpio_direction_file = fopen(GPIO_30_DIRECTION, "w");
	if (gpio_direction_file == NULL) {
		printf("Error setting gpio_direction_file initialization\n");
		exit(1);
	}

	if(fprintf(gpio_direction_file, "out") <= 0) {
		printf("Error setting gpio direction value\n");
		exit(1);
	}

	fclose(gpio_direction_file);


	long seconds = 1;
	long nanoseconds = 500000000;

	struct timespec delayReq = {seconds, nanoseconds};

	FILE *gpio_value = fopen(GPIO_30_VALUE, "w");
	if(gpio_value == NULL) {
		printf("Error setting gpio_value initialization\n");
		exit(1);
	}

	if(fprintf(gpio_value, "1") <= 0) {
		printf("Error setting gpio value\n");
		exit(1);
	}

	nanosleep(&delayReq, (struct timespec *) NULL);
	fclose(gpio_value);

	gpio_value = fopen(GPIO_30_VALUE, "w");
	if(gpio_value == NULL) {
		printf("Error setting gpio_value initialization\n");
		exit(1);
	}

	if(fprintf(gpio_value, "0") <= 0) {
		printf("Error setting gpio value\n");
		exit(1);
	}

	nanosleep(&delayReq, (struct timespec *) NULL);
	fclose(gpio_value);


	gpio_value = fopen(GPIO_30_VALUE, "w");
	if(gpio_value == NULL) {
		printf("Error setting gpio_value initialization\n");
		exit(1);
	}

	if(fprintf(gpio_value, "1") <= 0) {
		printf("Error setting gpio value\n");
		exit(1);
	}

	nanosleep(&delayReq, (struct timespec *) NULL);
	fclose(gpio_value);

	gpio_value = fopen(GPIO_30_VALUE, "w");
	if(gpio_value == NULL) {
		printf("Error setting gpio_value initialization\n");
		exit(1);
	}

	if(fprintf(gpio_value, "0") <= 0) {
		printf("Error setting gpio value\n");
		exit(1);
	}

	nanosleep(&delayReq, (struct timespec *) NULL);
	fclose(gpio_value);

	gpio_value = fopen(GPIO_30_VALUE, "w");
	if(gpio_value == NULL) {
		printf("Error setting gpio_value initialization\n");
		exit(1);
	}

	if(fprintf(gpio_value, "1") <= 0) {
		printf("Error setting gpio value\n");
		exit(1);
	}

	nanosleep(&delayReq, (struct timespec *) NULL);
	fclose(gpio_value);

	gpio_value = fopen(GPIO_30_VALUE, "w");
	if(gpio_value == NULL) {
		printf("Error setting gpio_value initialization\n");
		exit(1);
	}

	if(fprintf(gpio_value, "0") <= 0) {
		printf("Error setting gpio value\n");
		exit(1);
	}
	
	nanosleep(&delayReq, (struct timespec *) NULL);
	fclose(gpio_value);


	return 0;
}