#include <stdio.h>
#include <stdlib.h>

static const char *HELP = "Accepted command examples:\n"
						  "count -- display total number of samples taken.\n"
						  "length -- display number of samples in history (both max, and current).\n"
						  "history -- display the full sample history being saved.\n"
						  "get 10 -- display the 10 most recent history values.\n"
						  "dips -- display number of .\n"
						  "stop -- cause the server program to end.\n"
	   					  "<enter> -- repeat last command.\n";

int getHelpString(char **str) {
	int strLen = snprintf(NULL, 0, "%s", HELP) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen, "%s", HELP);
	return strLen;
}

int getCountString(char **str, int count) {
	int strLen = snprintf(NULL, 0, "Number of samples taken = %d.\n", count) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen, "Number of samples taken = %d.\n", count);
	return strLen;
}

int getLengthString(char **str, int maxSize, int currentSize) {
	int strLen = snprintf(NULL, 0, "History can hold  %d samples.\n"
								   "Currently holding %d samples.\n", maxSize, currentSize) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen, "History can hold  %d samples.\n"
						   "Currently holding %d samples.\n", maxSize, currentSize);
	return strLen;
}

int getHistoryString(char **str, double *data, int dataSize, int displaySize) {
	int strLen = 0;
	int newLineCount = 1;
	for(int i = dataSize - displaySize; i < dataSize; i++) {
		if(newLineCount % 20 == 0) {
			strLen += snprintf(NULL, 0, "%0.3f,\n", data[i]);
		} else {
			strLen += snprintf(NULL, 0, "%0.3f, ", data[i]);
		}
		newLineCount++;
	}

	*str = malloc(sizeof(char) * (strLen + 1)) + 1;
	int strOffset = 0;
	newLineCount = 1;
	for(int i = dataSize - displaySize; i < dataSize; i++) {
		if(newLineCount % 20 == 0) {
			strOffset += snprintf(*str + strOffset, strLen - strOffset, "%0.3f,\n", data[i]);
		} else {
			strOffset += snprintf(*str + strOffset, strLen - strOffset, "%0.3f, ", data[i]);
		}
		newLineCount++;
	}

	return strLen;
}

int getDipString(char **str, int dipCount) {
	int strLen = snprintf(NULL, 0, "# Dips = %d.\n", dipCount) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen + 1, "# Dips = %d.\n", dipCount);
	return strLen;
}

int main() {
	char *test;
	double arr[40];
	for(int i = 0; i < 40; i++) {
		arr[i] = i;
	}
	getHistoryString(&test, arr, 40, 0);
	printf("%s\n", test);
	
	free(test);
	return 0;
}