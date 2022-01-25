#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "joyStickControl.h"  // Importing custom methods for interacting with GPIO joystick
#include "ledControl.h"       // Importing custom methods forintereacting with beaglebon LED 0~3
#include "directions.h"       // Importing enum {LEFT, RIGHT, UP, DOWN}


/*
Returns UP or DOWN by random
input: void
output: enum
*/
directions pickDirections() {
	int randomNum = rand() % 2;
			
	directions answer;
	if(randomNum == 0) {
		answer = UP;
	} else {
		answer = DOWN;
	}

	return answer;
}



int main() {
	// Set LED 0~3 to allow for direct control
	initializeLedTriggers();
	turnOffAllLeds();

	printf("Hello embedded world, from Yoonhong!\n\n");
	printf("Press the Zen cape's Joystick in the direction of the LED.\n");
	printf("\tUP for LED 0 (top)\n");
	printf("\tDOWN for LED 3 (bottom)\n");
	printf("\tLEFT/RIGHT for exit app.\n");

	int totalAttempts = 0;
	int correctAttempts = 0;

	while (true) {
		printf("Press joystick; current score (%d / %d)\n", correctAttempts, totalAttempts);
		
		//Get direction from GPIO joystick input
		directions correctDirection = pickDirections();
		
		turnOnLed(correctDirection);
		
		// Turn on LED 0 or 3 based on GPIO joystick input
		directions chosenDirection = getDirections();
		
		if (chosenDirection == LEFT || chosenDirection == RIGHT) { // If the player moves the joystick to left to right exit out of the loop
			break;
		} else if (chosenDirection == UP || chosenDirection == DOWN) { // If the player moves the joystick to up or down, check if the guess is correct
			if(chosenDirection == correctDirection) {
				// Increase current score by one
				correctAttempts++;

				// Flash onces for 0.1s
				printf("Correct!\n");
				flashLeds(100000000, 1); 
			} else {
				// Flash 5 times (0.1s long for each flash)
				printf("Incorrect! :(\n");
				flashLeds(100000000, 5);
			}
		} else {
			printf("Something went wrong\n");
		}

		// Increate total user attempts
		totalAttempts++;

	}

	// Player ended the game, turn off all leds and display total score
	turnOffAllLeds();
	printf("Your final score was (%d / %d)\n", correctAttempts, totalAttempts);
	printf("Thank you for playing!\n");

	return 0;
}