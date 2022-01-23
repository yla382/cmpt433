#include <stdio.h>
#include <time.h>
#include "fileHandler.h" // importing file read/write methods
#include "directions.h"  // Importing enum {LEFT, RIGHT, UP, DOWN}

void initializeLedTriggers(void);
void turnOffAllLeds(void);
void turnOnLed(directions direction);
void flashLeds(long duration, int repetition);