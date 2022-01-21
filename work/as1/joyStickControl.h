#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileHandler.h"
#include "directions.h"

// typedef enum {LEFT, RIGHT, UP, DOWN} directions;
directions getDirections(void);