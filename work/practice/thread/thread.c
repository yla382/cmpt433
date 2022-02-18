#include <stdio.h>
#include <pthread.h>


void *printLessThan3(int *val) {
	while(true) {
		int currVal = *val
		if(currVal <= 3) {
			printf("Thread 1: %d\n");
			(*val)++;
		}
	}
}

void *printGreaterThan3(int *val) {
	while(true) {
		int currVal = *val
		if(currVal > 3) {
			printf("Thread 2: %d\n");
			(*val)--;
		}
	}
}

int main() {
	printf("Thread Example\n");
	return 0;
}