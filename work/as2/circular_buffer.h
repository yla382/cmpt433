#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

typedef struct {
	int size;
	double *array;
	int start;
	int end;
	int currentSize;
	int historicCount;
	int historicSum;
	bool isFull;
} CircularBuffer;

CircularBuffer initializeBuffer(int bufferSize);

int getBufferSize(CircularBuffer buffer);

bool isBufferFull(CircularBuffer buffer);

void insertNum(CircularBuffer *buffer, double val);

double *getArray(CircularBuffer buffer, double *arr);

void resizeBuffer(CircularBuffer *buffer, int newSize);

void displayBuffer(CircularBuffer buffer);

#endif