#include "circular_buffer.h"

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
			if(buffer->end + 1 >= (buffer->size) - 1) {
				buffer->isFull = true;
			} 
			(buffer->currentSize)++;
			(buffer->end)++;
			(buffer->array)[buffer->end] = val;
		}
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
	int sizeDiff = (buffer->size) - newSize;
	buffer->array = malloc(newSize * sizeof(double));
	buffer->start = 0;
	buffer->size = newSize;
	
	if(buffer->size > newSize) {
		buffer->currentSize = newSize;
		buffer->end = newSize - 1;

		for(int i = 0; i < newSize; i++) {
			(buffer->array)[i] = currentArr[sizeDiff + i];
		}
	} else {
		buffer->end = (buffer->currentSize) - 1;

		for(int i = 0; i < buffer->currentSize; i++) {
			(buffer->array)[i] = currentArr[i];
		}
	}
}

void displayBuffer(CircularBuffer buffer) {
	if(buffer.start == -1 && buffer.end == -1) {
		printf("Buffer is empty");
		return;
	}

	if(buffer.start <= buffer.end) {
		for(int i = buffer.start; i <= buffer.end; i++) {
			printf("%5.2f ", (buffer.array)[i]);
		}
	} else {
		for(int i = buffer.start; i < buffer.size; i++) {
			printf("%5.2f ", (buffer.array)[i]);
		}

		for(int i = 0; i <= buffer.end; i++) {
			printf("%5.2f ", (buffer.array[i]));
		}
	}

	printf("\n");
}