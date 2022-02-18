#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

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


CircularBuffer initializeBuffer(int bufferSize) {
	CircularBuffer buffer;
	buffer.size = bufferSize;
	buffer.array = malloc(sizeof(double) * bufferSize);
	buffer.start = -1;
	buffer.end = -1;
	buffer.currentSize = 0;
	buffer.historicCount = 0;
	buffer.historicSum = 0;
	buffer.isFull = false;

	return buffer;
}

int getBufferSize(CircularBuffer buffer) {
	// printf("Current Buffer Size: %d\n", buffer.size);
	return buffer.size;
}


bool isBufferFull(CircularBuffer buffer) {
	//printf("Buffer Full: %s\n", buffer.isFull ? "true" : "false");
	return buffer.isFull;
}

void insertNum(CircularBuffer *buffer, double val) {
	if(buffer->start == -1 && buffer->end == -1) {
		//buffer is empty
		(buffer->currentSize)++;
		buffer->start = 0;
		buffer->end = 0;
		(buffer->array)[0] = val;
	} else {
		if(buffer->isFull) {
			buffer->end = (buffer->end) + 1 >= buffer->size ? 0 : buffer->end + 1;
			buffer->start = (buffer->start) + 1 >= buffer->size ? 0 : buffer->start + 1; 
			(buffer->array)[buffer->end] = val;
		} else {
			(buffer->currentSize)++;
			(buffer->end)++;
			(buffer->array)[buffer->end] = val;
		}
	}

	if(buffer->currentSize >= buffer->size) {
		buffer->isFull = true;
	} else {
		buffer->isFull = false;
	}

	(buffer->historicCount)++;
	buffer->historicSum += val;

}

double *getArray(CircularBuffer buffer, double *arr) {
	int counter = 0;
	if(buffer.start == -1 && buffer.end == -1) {
		return arr;
	}

	if(buffer.start <= buffer.end) {
		for(int i = buffer.start; i <= buffer.end; i++) {
			arr[counter] = (buffer.array)[i];
			counter++;
		}
	} else {
		for(int i = buffer.start; i < buffer.size; i++) {
			arr[counter] = (buffer.array)[i];
			counter++;
		}

		for(int i = 0; i <= buffer.end; i++) {
			arr[counter] = (buffer.array)[i];
			counter++;
		}
	}

	return arr;
}

void resizeBuffer(CircularBuffer *buffer, int newSize) {
	if (buffer->size == newSize) {
		return;
	}

	if (buffer->currentSize == 0) {
		free(buffer->array);
		buffer->array = malloc(newSize * sizeof(double));
		buffer->start = -1;
		buffer->end = -1;
		buffer->size = newSize;
		return;
	}

	double currentArr[buffer->currentSize];
	getArray(*buffer, currentArr);
	  
	free(buffer->array);
	buffer->array = malloc(newSize * sizeof(double));
	buffer->start = 0;
  
  if(newSize <= buffer->currentSize) {
  	int sizeDiff = (buffer->currentSize) - newSize;
  	int i;
  	printf("SizeDiff: %d\n", sizeDiff);
  	for(i = sizeDiff; i < buffer->currentSize; i++) {
  		(buffer->array)[i - sizeDiff] = currentArr[i];
  	}

  	buffer->currentSize = i - sizeDiff;
  	buffer->end = i - sizeDiff - 1;
  } else {
  	for(int i = 0; i < buffer->currentSize; i++) {
  		(buffer->array)[i] = currentArr[i];
  	}
  	buffer->end = (buffer->currentSize) - 1;
  }

  buffer->size = newSize;

	if(buffer->currentSize >= buffer->size) {
		buffer->isFull = true;
	} else {
		buffer->isFull = false;
	}
}


void displayBuffer(CircularBuffer buffer, int skip) {
	int currentSize = buffer.currentSize;
	printf("currentSize: %d\n", currentSize);
	printf("start: %d\n", buffer.start);
	printf("end: %d\n", buffer.end);
	printf("isFull: %s\n", buffer.isFull ? "true":"false");
	double currentArr[currentSize];
	getArray(buffer, currentArr);
	
	for(int i = 0; i < currentSize; i+=skip) {
		printf("%0.2f  ", currentArr[i]);
	}

	printf("\n\n");
}




int main() {
	printf("Circular Buffer Initializing...\n");
	CircularBuffer buff = initializeBuffer(5);

	for(int i = 0; i < 5; i++) {
		insertNum(&buff, i);
	}

	insertNum(&buff, 5);
	insertNum(&buff, 6);

	
	displayBuffer(buff, 1);
	resizeBuffer(&buff, 7);
	for(int i = 7; i < 15; i++) {
		insertNum(&buff, i);
		displayBuffer(buff, 1);
	}

	
	printf("\n");
	return 0;
}