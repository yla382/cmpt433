#include "command_string.h"

static const char *HELP = "Accepted command examples:\n"
						  "count    -- display total number of samples taken.\n"
						  "length   -- display number of samples in history (both max, and current).\n"
						  "history  -- display the full sample history being saved.\n"
						  "get 10   -- display the 10 most recent history values.\n"
						  "dips     -- display number of .\n"
						  "stop     -- cause the server program to end.\n"
	   					  "<enter>  -- repeat last command.\n";


/*
Convert every characters in a string into upper case
input: *char, int
output: int
*/
void stringUpper(char *str, int strLen) {
	for(int i = 0; i < strLen; i++) {
		str[i] = toupper(str[i]);
	}
}


/*
Split input string by space and extract first and second string
returns number of strings separated
input: *char, **char, **char
*/
int extractRequest(char *str, char **arg1, char **arg2) {
	char separator[] = " "; 
	//get first string by converting first space into 0
	char *sepPointer = strtok(str, separator);
	*arg1 = sepPointer;
	
	int sepCount = 1;
	bool arg2Defined = false;
	//loop through rest of the input to separte string by space
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


/*
allocate dynamic memory containing copy of HELP string
input: **char
output: int
*/
int getHelpString(char **str) {
	//Get required size for reponse string
	int strLen = snprintf(NULL, 0, "%s", HELP) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen, "%s", HELP);
	return strLen;
}


/*
allocate dynamic memory for Count request's response
ex. "Number of samples taken = 10"
input: **char, int
output: int
*/
int getCountString(char **str, int count) {
	//Get required size for reponse string
	int strLen = snprintf(NULL, 0, "Number of samples taken = %d.\n", count) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen, "Number of samples taken = %d.\n", count);
	return strLen;
}


/*
allocate dynamic memory for Length request's response
ex. "History can hold  500 samples."
	"Currently holding 200 samples."
input: **char, int
output: int
*/
int getLengthString(char **str, int maxSize, int currentSize) {
	//Get required size for reponse string
	int strLen = snprintf(NULL, 0, "History can hold  %d samples.\n"
								   "Currently holding %d samples.\n", maxSize, currentSize) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen, "History can hold  %d samples.\n"
						   "Currently holding %d samples.\n", maxSize, currentSize);
	return strLen;
}


/*
allocate dynamic memory for Length History and Get's response
ex. "0.123, 0.234, ...."
input: **char, int
output: int
*/
int getHistoryString(char **str, double *data, int dataSize, int displaySize) {
	int strLen = 0;
	int newLineCount = 1;
	//Get required size for reponse string + required spaces, commas and newlines
	for(int i = dataSize - displaySize; i < dataSize; i++) {
		if(newLineCount % 20 == 0) {
			strLen += snprintf(NULL, 0, "%0.3f,\n", data[i]);
		} else {
			strLen += snprintf(NULL, 0, "%0.3f, ", data[i]);
		}
		newLineCount++;
	}

	*str = malloc(sizeof(char) * (strLen + 2)); // +2 for newline at the end of string and NULL terminator
	int strOffset = 0;
	newLineCount = 1;
	for(int i = dataSize - displaySize; i < dataSize; i++) {
		// Every 20 numbers or last number add newlines
		if(newLineCount % 20 == 0 || i == dataSize - 1) {
			strOffset += snprintf(*str + strOffset, (strLen + 2) - strOffset, "%0.3f,\n", data[i]);
		} else {
			strOffset += snprintf(*str + strOffset, (strLen + 2) - strOffset, "%0.3f, ", data[i]);
		}
		newLineCount++;
	}

	return strLen;
}


/*
allocate dynamic memory for Dips request's response
ex. "# Dips = 35."
input: **char, int
output: int
*/
int getDipString(char **str, int dipCount) {
	//Get required size for reponse string
	int strLen = snprintf(NULL, 0, "# Dips = %d.\n", dipCount) + 1;
	*str = malloc(sizeof(char) * strLen);
	snprintf(*str, strLen + 1, "# Dips = %d.\n", dipCount);
	return strLen;
}