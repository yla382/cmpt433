#include <stdio.h>
#include <time.h>
#include "fileHandler.h"
#include "directions.h"

void initializeLedTriggers(void);
void turnOffAllLeds(void);
void turnOnLed(directions direction);
void flashLeds(long duration, int repetition);