#include <stdio.h>
#include "light_sampler.h"

int main() {
	printf("Starting to sample data...\n");
	light_sampler(); //Start sampling light voltage from beaglebone
	printf("Cleaning up...4321\nDone!\n");
}