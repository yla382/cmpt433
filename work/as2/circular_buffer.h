#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

/*
Struct definition for circular array
*/
typedef struct {
	int size;          // size of the array
	double *array;     // array used for storing data in circular array
	int start;         // index of oldest data in an array. -1 if array is empty
	int end;           // index of newest data in an array, -1 if array is empty
	int currentSize;   // currentn number of items in an array
	int historicCount; // total number of items inserted to array since buffer initalization
	bool isFull;       // status of whether array is full or not
} CircularBuffer;


/*
Initialize CircularBuffer. CircularBuffer.array is dynamically created
input: int
output: struct
*/
CircularBuffer initializeBuffer(int bufferSize);


/*
Gets the size of the CircularBuffer.array
input: struct
output: int
*/
int getBufferSize(CircularBuffer buffer);


/*
Check if CircularBuffer.array is full
input: struct
output: bool
*/
bool isBufferFull(CircularBuffer buffer);


/*
Inserts new value to CircularBuffer. If its array is full, remove the
oldest item in the array to make room.
input: *struct, double
output: void
*/
void insertNum(CircularBuffer *buffer, double val);


/*
Copies array of items in the CircularBuffer from oldest to newest
input: struct, *double
output: double
*/
double *getArray(CircularBuffer buffer, double *arr);


/*
Resize the CircularBuffer.array. If new size < current size,
remove oldests items from the array
input: *struct, int
output: void
*/
void resizeBuffer(CircularBuffer *buffer, int newSize);


/*
Prints items from oldes to newst in CircularBuffer.array. Skips items based
on argument
input: struct, int
output: void
*/
void displayBuffer(CircularBuffer buffer, int skip);

#endif