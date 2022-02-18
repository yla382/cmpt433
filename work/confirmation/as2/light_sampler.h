#include "circular_buffer.h"
#include "a2d.h"
#include "display.h"
#include "command_string.h"
#include "network.h"

#ifndef LIGHT_SAMPLER_H
#define LIGHT_SAMPLER_H


/*
Samples light level voltage
Features:
	 - samples light level voltage
	 - read value from potentiometer to update sample history size
	 - calculate exponential average
	 - detect dips in light sensor
	 - display number of dips in 14 seg display
	 - UDP connection on port 12345 for remote query on samples
	 	- commands: help, count, length, get N, history, dips, stop
	 - uses 6 threads
input: void
output: void
*/
void light_sampler();

#endif