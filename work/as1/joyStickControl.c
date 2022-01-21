#include "joyStickControl.h"

#define MAX_EVENTS 4

#define DIRECTION_SETTING    "in"
#define EDGE_TRIGGER_SETTING "falling"

#define GPIO_DIRECTION_LEFT  "/sys/class/gpio/gpio65/direction"
#define GPIO_VALUE_LEFT      "/sys/class/gpio/gpio65/value"
#define GPIO_TRIGGER_LEFT    "/sys/class/gpio/gpio65/edge"

#define GPIO_DIRECTION_RIGHT "/sys/class/gpio/gpio47/direction"
#define GPIO_VALUE_RIGHT     "/sys/class/gpio/gpio47/value"
#define GPIO_TRIGGER_RIGHT    "/sys/class/gpio/gpio47/edge"

#define GPIO_DIRECTION_UP 	 "/sys/class/gpio/gpio26/direction"
#define GPIO_VALUE_UP        "/sys/class/gpio/gpio26/value"
#define GPIO_TRIGGER_UP    "/sys/class/gpio/gpio26/edge"

#define GPIO_DIRECTION_DOWN	 "/sys/class/gpio/gpio46/direction"
#define GPIO_VALUE_DOWN      "/sys/class/gpio/gpio46/value"
#define GPIO_TRIGGER_DOWN    "/sys/class/gpio/gpio46/edge"


// void setFile(char *fileName, char *value) {
// 	FILE *file = fopen(fileName, "w");
	
// 	if (file == NULL) {
// 		printf("Error: unable to write file %s\n", fileName);
// 		exit(1);
// 	}

// 	fprintf(file, "%s", value);
// 	fclose(file);
// }

// char getFileContent(char *fileName) {
// 	FILE *file = fopen(fileName, "r");

// 	if (file == NULL) {
// 		printf("Error: unable to read from file %s\n", fileName);
// 	}

// 	char stringBuf[MAX_LENGTH];
// 	fgets(stringBuf, MAX_LENGTH, file);
// 	fclose(file);

// 	return stringBuf[0];
// }

int gpioEdgeTrigger(char** fileNameForGpioValues, int size) {
	int epollfd = epoll_create(1);
	
	if (epollfd == -1) {
		return -1;
	}

	struct epoll_event events[MAX_EVENTS], epoll_struct[MAX_EVENTS];
	int fd_Array[size];

	for(int i = 0; i < size; i++) {
		fd_Array[i] = open(fileNameForGpioValues[i], O_RDONLY | O_NONBLOCK);
		if (fd_Array[i] == -1) {
			return - 1;
		}

		epoll_struct[i].events = EPOLLIN | EPOLLET | EPOLLPRI;
		epoll_struct[i].data.fd = fd_Array[i];

		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_Array[i], &epoll_struct[i]) == -1) {
			return -1;
		}
	}

	for (int i = 0; i < 2; i++) {
		int availableFds = epoll_wait(epollfd, events, 4, -1);

		if (availableFds == -1){
			for(int i = 0; i < size; i++) {
				close(fd_Array[i]);
			}
			close(epollfd);
			return -1;
		}

	}

	return 0;
}


directions getDirections() {
	// set joystick direction for input
	setFile(GPIO_DIRECTION_LEFT, DIRECTION_SETTING);
	setFile(GPIO_DIRECTION_RIGHT, DIRECTION_SETTING);
	setFile(GPIO_DIRECTION_UP, DIRECTION_SETTING);
	setFile(GPIO_DIRECTION_DOWN, DIRECTION_SETTING);

	setFile(GPIO_TRIGGER_LEFT, EDGE_TRIGGER_SETTING);
	setFile(GPIO_TRIGGER_RIGHT, EDGE_TRIGGER_SETTING);
	setFile(GPIO_TRIGGER_UP, EDGE_TRIGGER_SETTING);
	setFile(GPIO_TRIGGER_DOWN, EDGE_TRIGGER_SETTING);

	char *gpioFileNames[] = {
	    GPIO_VALUE_LEFT,
	    GPIO_VALUE_RIGHT,
	    GPIO_VALUE_UP,
	    GPIO_VALUE_DOWN
	};

	while (true) {
		int triggerResult = gpioEdgeTrigger(gpioFileNames, MAX_EVENTS);
		if (triggerResult == -1) {
			printf("Something went wrong with epoll implementes\n");
			exit(1);
		}

		char leftVal = getFileContent(GPIO_VALUE_LEFT);
		if (leftVal == '0') {
			return LEFT;
		}

		char rightVal = getFileContent(GPIO_VALUE_RIGHT);
		if (rightVal == '0') {
			return RIGHT;
		}

		char upVal = getFileContent(GPIO_VALUE_UP);
		if (upVal == '0') {
			return UP;
		}

		char downVal = getFileContent(GPIO_VALUE_DOWN);
		if (downVal == '0') {
			return DOWN;
		}
	}
}
