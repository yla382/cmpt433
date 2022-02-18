#include "circular_buffer.h"

/*
Initialize CircularBuffer. CircularBuffer.array is dynamically created
input: int
output: struct
*/
CircularBuffer initializeBuffer(int bufferSize) {
	CircularBuffer buffer;
	buffer.size = bufferSize;
	buffer.array = malloc(sizeof(double) * bufferSize);
	buffer.start = -1;
	buffer.end = -1;
	buffer.currentSize = 0;
	buffer.historicCount = 0;
	buffer.isFull = false;

	return buffer;
}


/*
Gets the size of the CircularBuffer.array
input: struct
output: int
*/
int getBufferSize(CircularBuffer buffer) {
	return buffer.size;
}


/*
Check if CircularBuffer.array is full
input: struct
output: bool
*/
bool isBufferFull(CircularBuffer buffer) {
	return buffer.isFull;
}


/*
Inserts new value to CircularBuffer. If its array is full, remove the
oldest item in the array to make room.
input: *struct, double
output: void
*/
void insertNum(CircularBuffer *buffer, double val) {
	if(buffer->start == -1 && buffer->end == -1) {
		//When buffer is empty
		(buffer->currentSize)++;
		buffer->start = 0;
		buffer->end = 0;
		(buffer->array)[0] = val;
	} else {
		if(buffer->isFull) {
			//If buffer is shift start and end index to right
			//if the one of the indexes are at the last location, set to 0
			buffer->end = (buffer->end) + 1 >= buffer->size ? 0 : buffer->end + 1;
			buffer->start = (buffer->start) + 1 >= buffer->size ? 0 : buffer->start + 1; 
			(buffer->array)[buffer->end] = val;
		} else {
			//Buffer is not full, just add value to next available position
			(buffer->currentSize)++;
			(buffer->end)++;
			(buffer->array)[buffer->end] = val;
		}
	}

	//Check if the buffer is full after insert new item
	if(buffer->currentSize >= buffer->size) {
		buffer->isFull = true;
	} else {
		buffer->isFull = false;
	}

	(buffer->historicCount)++;
}


/*
Copies array of items in the CircularBuffer from oldest to newest
input: struct, *double
output: double
*/
double *getArray(CircularBuffer buffer, double *arr) {
	int counter = 0;

	//Buffer empty, do nothing
	if(buffer.start == -1 && buffer.end == -1) {
		return arr;
	}

	//start <= end, just look from start to end
	if(buffer.start <= buffer.end) {
		for(int i = buffer.start; i <= buffer.end; i++) {
			arr[counter] = (buffer.array)[i];
			counter++;
		}
	} else {
		//Buffer is full and end < start
		//                 end start         
		//                  |  | 
		// ex. [0][1][2][3][4][5][6][7]

		//loop from start to end of array
		for(int i = buffer.start; i < buffer.size; i++) {
			arr[counter] = (buffer.array)[i];
			counter++;
		}

		//loop from 0 to end index
		for(int i = 0; i <= buffer.end; i++) {
			arr[counter] = (buffer.array)[i];
			counter++;
		}
	}

	return arr;
}


/*
Resize the CircularBuffer.array. If new size < current size,
remove oldests items from the array
input: *struct, int
output: void
*/
void resizeBuffer(CircularBuffer *buffer, int newSize) {
	//Do nothing if there is no change in buffer size
	if (buffer->size == newSize) {
		return;
	}

	//if buffer is empty, just re-inistalized buffer is new size
	if (buffer->currentSize == 0) {
		free(buffer->array);
		buffer->array = malloc(newSize * sizeof(double));
		buffer->start = -1; //indicate buffer is empty
		buffer->end = -1;   //indicate buffer is empty
		buffer->size = newSize;
		return;
	}

	//Get copy of current buffer in order
	double currentArr[buffer->currentSize];
	getArray(*buffer, currentArr);
	
	//de-allocate current array and re-allocate with new size  
	free(buffer->array);
	buffer->array = malloc(newSize * sizeof(double));
	buffer->start = 0;
  	
  	//when new size is less than current number of element in array,
  	//only copy from (buffer->currentSize) - newSize to current size
	if(newSize <= buffer->currentSize) {
  		int sizeDiff = (buffer->currentSize) - newSize;
  		int i;
  		for(i = sizeDiff; i < buffer->currentSize; i++) {
  			(buffer->array)[i - sizeDiff] = currentArr[i];
  		}

  		buffer->currentSize = i - sizeDiff;
  		buffer->end = i - sizeDiff - 1;
  	} else {
  		//Copy everything from old array
  		for(int i = 0; i < buffer->currentSize; i++) {
  			(buffer->array)[i] = currentArr[i];
  		}
  		buffer->end = (buffer->currentSize) - 1;
  	}

  	buffer->size = newSize;

  	//Check if new array is full or not
	if(buffer->currentSize >= buffer->size) {
		buffer->isFull = true;
	} else {
		buffer->isFull = false;
	}
}



/*
Prints items from oldes to newst in CircularBuffer.array. Skips items based
on argument
input: struct, int
output: void
*/
void displayBuffer(CircularBuffer buffer, int skip) {
	int currentSize = buffer.currentSize;
	double currentArr[currentSize];
	getArray(buffer, currentArr);
	
	for(int i = 0; i < currentSize; i+=skip) {
		printf("%0.3f  ", currentArr[i]);
	}

	printf("\n");
}