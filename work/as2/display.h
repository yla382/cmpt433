#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#ifndef DISPLAY_H
#define DISPLAY_H

void initializeDisplay();
void closeDisplay();
void displayScreen(char *value);


#endif