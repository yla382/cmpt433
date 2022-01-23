#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileHandler.h" // importing file read/write methods
#include "directions.h"  // Importing enum {LEFT, RIGHT, UP, DOWN}

/*
Return the direction of joystick pressed
input: void
output: enum
*/
directions getDirections(void);