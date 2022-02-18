#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef A2D_H
#define A2D_H


/*
Get current raw value of the potentiometer (uses average value)
input: void
output: int
*/
int getPotReading(void);

/*
Get voltage of the current light level from light sensor
input: void
output: double
*/
double getLightLevelVoltage(void);

#endif