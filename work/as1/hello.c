#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "joyStickControl.h"
#include "ledControl.h"
#include "directions.h"

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
		
		directions correctDirection = pickDirections();
		turnOnLed(correctDirection);
		directions chosenDirection = getDirections();
		
		if (chosenDirection == LEFT || chosenDirection == RIGHT) {
			// turn off BBG’s LEDs
			break;
		} else if (chosenDirection == UP || chosenDirection == DOWN) {
			if(chosenDirection == correctDirection) {
				correctAttempts++;
				flashLeds(100000000, 1);
				printf("Correct!\n");
			} else {
				flashLeds(100000000, 5);
				printf("Incorrect! :(\n");
			}
		} else {
			printf("Something went wrong\n");
		}

		totalAttempts++;

	}

	turnOffAllLeds();
	printf("Your final score was (%d / %d)\n", correctAttempts, totalAttempts);
	printf("Thank you for playing!\n");

	return 0;
}