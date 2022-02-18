#include <stdio.h>
#include <time.h>
#include "fileHandler.h" // importing file read/write methods
#include "directions.h"  // Importing enum {LEFT, RIGHT, UP, DOWN, INVALID}

// Set LED 0~3's trigger setting to None
void initializeLedTriggers(void);

// Turn off LED 0~3
void turnOffAllLeds(void);

/*
Turn LED 0 on if direction is UP and turn on LED 3 with direction is DOWN
input: enum
output: void
*/
void turnOnLed(directions direction);

/*
Flash LED 0~3 with duration and number of repetitions
input: long, int
output: void
*/
void flashLeds(long duration, int repetition);