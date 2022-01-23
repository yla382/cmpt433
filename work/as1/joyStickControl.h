#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileHandler.h" // importing file read/write methods
#include "directions.h"  // Importing enum {LEFT, RIGHT, UP, DOWN}

// typedef enum {LEFT, RIGHT, UP, DOWN} directions;
directions getDirections(void);