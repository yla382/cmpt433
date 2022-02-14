#include "command_string.h"

static const char *HELP = "Accepted command examples:\n"
						  "count    -- display total number of samples taken.\n"
						  "length   -- display number of samples in history (both max, and current).\n"
						  "history  -- display the full sample history being saved.\n"
						  "get 10   -- display the 10 most recent history values.\n"
						  "dips     -- display number of .\n"
						  "stop     -- cause the server program to end.\n"
	   					  "<enter>  -- repeat last command.\n";

void stringUpper(char *str, int strLen) {
	for(int i = 0; i < strLen; i++) {
		str[i] = toupper(str[i]);
	}
}

int extractRequest(char *str, char **arg1, char **arg2) {
	char separator[] = " "; 
	char *sepPointer = strtok(str, separator);
	*arg1 = sepPointer;

	int sepCount = 1;
	bool arg2Defined = false;
	while(sepPointer != NULL) {
		sepPointer = strtok(NULL, separator);
		if(!arg2Defined) {
			arg2Defined = true;
			*arg2 = sepPointer;
		}
		sepCount++;
	}

	return sepCount;
}


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

	*str = malloc(sizeof(char) * (strLen + 2));
	int strOffset = 0;
	newLineCount = 1;
	for(int i = dataSize - displaySize; i < dataSize; i++) {
		if(newLineCount % 20 == 0 || i == dataSize - 1) {
			strOffset += snprintf(*str + strOffset, (strLen + 2) - strOffset, "%0.3f,\n", data[i]);
		} else {
			strOffset += snprintf(*str + strOffset, (strLen + 2) - strOffset, "%0.3f, ", data[i]);
		}
		newLineCount++;
	}

	printf("test\n");
	printf("%s", *str);

	return strLen;
}

int getDipString(char **str, int dipCount) {
	int strLen = snprintf(NULL, 0, "# Dips = %d.\n", dipCount) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen + 1, "# Dips = %d.\n", dipCount);
	return strLen;
}