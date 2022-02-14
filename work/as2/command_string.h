#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifndef COMMAND_STRING_H
#define COMMAND_STRING_H

void stringUpper(char *str, int strLen);
int extractRequest(char *str, char **arg1, char **arg2);
int getHelpString(char **str);
int getCountString(char **str, int count);
int getLengthString(char **str, int maxSize, int currentSize);
int getHistoryString(char **str, double *data, int dataSize, int displaySize);
int getDipString(char **str, int dipCount);

#endif