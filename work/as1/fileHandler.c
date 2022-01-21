#include "fileHandler.h"

#define MAX_LENGTH 1024

void setFile(char *fileName, char *value) {
	FILE *file = fopen(fileName, "w");
	
	if (file == NULL) {
		printf("Error: unable to write file %s\n", fileName);
		exit(1);
	}

	fprintf(file, "%s", value);
	fclose(file);
}

char getFileContent(char *fileName) {
	FILE *file = fopen(fileName, "r");

	if (file == NULL) {
		printf("Error: unable to read from file %s\n", fileName);
	}

	char stringBuf[MAX_LENGTH];
	fgets(stringBuf, MAX_LENGTH, file);
	fclose(file);

	return stringBuf[0];
}