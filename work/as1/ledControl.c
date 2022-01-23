#include "ledControl.h"

// LED 0 related file locations
#define LED_TRIGGER_0    "/sys/class/leds/beaglebone:green:usr0/trigger"
#define LED_BRIGHTNESS_0 "/sys/class/leds/beaglebone:green:usr0/brightness"

// LED 1 related file locations
#define LED_TRIGGER_1    "/sys/class/leds/beaglebone:green:usr1/trigger"
#define LED_BRIGHTNESS_1 "/sys/class/leds/beaglebone:green:usr1/brightness"

// LED 2 related file locations
#define LED_TRIGGER_2    "/sys/class/leds/beaglebone:green:usr2/trigger"
#define LED_BRIGHTNESS_2 "/sys/class/leds/beaglebone:green:usr2/brightness"

// LED 3 related file locations
#define LED_TRIGGER_3    "/sys/class/leds/beaglebone:green:usr3/trigger"
#define LED_BRIGHTNESS_3 "/sys/class/leds/beaglebone:green:usr3/brightness"


// Set LED 0~3's trigger setting to None
void initializeLedTriggers() {
	setFile(LED_TRIGGER_0, "none");
	setFile(LED_TRIGGER_1, "none");
	setFile(LED_TRIGGER_2, "none");
	setFile(LED_TRIGGER_3, "none");
}


// Turn on LED 0~3
void turnOnAllLeds() {
	setFile(LED_BRIGHTNESS_0, "1");
	setFile(LED_BRIGHTNESS_1, "1");
	setFile(LED_BRIGHTNESS_2, "1");
	setFile(LED_BRIGHTNESS_3, "1");
}

// Turn off LED 0~3
void turnOffAllLeds() {
	setFile(LED_BRIGHTNESS_0, "0");
	setFile(LED_BRIGHTNESS_1, "0");
	setFile(LED_BRIGHTNESS_2, "0");
	setFile(LED_BRIGHTNESS_3, "0");
}


/*
Turn LED 0 on if direction is UP and turn on LED 3 with direction is DOWN
input: enum
output: void
*/
void turnOnLed(directions direction) {
	if (direction == UP) {
		setFile(LED_BRIGHTNESS_0, "1");
	} else if (direction == DOWN) {
		setFile(LED_BRIGHTNESS_3, "1");
	} else {
		turnOffAllLeds();
	}
}


/*
Flash LED 0~3 with duration and number of repetitions
input: long, int
output: void
*/
void flashLeds(long duration, int repetition) {
	for(int i = 0; i < repetition; i++) {
		turnOnAllLeds();

		struct timespec timer = {0, duration};
		nanosleep(&timer, (struct timespec *) NULL);

		turnOffAllLeds();
		nanosleep(&timer, (struct timespec *) NULL);
	}
}