#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifndef COMMAND_STRING_H
#define COMMAND_STRING_H


/*
Convert every characters in a string into upper case
input: *char, int
output: int
*/
void stringUpper(char *str, int strLen);


/*
Split input string by space and extract first and second string
returns number of strings separated
input: *char, **char, **char
*/
int extractRequest(char *str, char **arg1, char **arg2);


/*
allocate dynamic memory containing copy of HELP string
input: **char
output: int
*/
int getHelpString(char **str);


/*
allocate dynamic memory for Count request's response
ex. "Number of samples taken = 10"
input: **char, int
output: int
*/
int getCountString(char **str, int count);


/*
allocate dynamic memory for Length request's response
ex. "History can hold  500 samples."
	"Currently holding 200 samples."
input: **char, int
output: int
*/
int getLengthString(char **str, int maxSize, int currentSize);


/*
allocate dynamic memory for Length History and Get's response
ex. "0.123, 0.234, ...."
input: **char, int
output: int
*/
int getHistoryString(char **str, double *data, int dataSize, int displaySize);


/*
allocate dynamic memory for Dips request's response
ex. "# Dips = 35."
input: **char, int
output: int
*/
int getDipString(char **str, int dipCount);

#endif