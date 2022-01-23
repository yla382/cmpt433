#include "joyStickControl.h"

// Number of possible events for GPIO joystick
#define MAX_EVENTS 4

// GPIO joystick direction setting value
#define DIRECTION_SETTING    "in"

// GPIO joystick trigger setting value
#define EDGE_TRIGGER_SETTING "falling"

// GPIO 65(left) related file locations
#define GPIO_DIRECTION_LEFT  "/sys/class/gpio/gpio65/direction"
#define GPIO_VALUE_LEFT      "/sys/class/gpio/gpio65/value"
#define GPIO_TRIGGER_LEFT    "/sys/class/gpio/gpio65/edge"

// GPIO 47(right) related file locations
#define GPIO_DIRECTION_RIGHT "/sys/class/gpio/gpio47/direction"
#define GPIO_VALUE_RIGHT     "/sys/class/gpio/gpio47/value"
#define GPIO_TRIGGER_RIGHT    "/sys/class/gpio/gpio47/edge"

// GPIO 26(up) related file locations
#define GPIO_DIRECTION_UP 	 "/sys/class/gpio/gpio26/direction"
#define GPIO_VALUE_UP        "/sys/class/gpio/gpio26/value"
#define GPIO_TRIGGER_UP    "/sys/class/gpio/gpio26/edge"

// GPIO 46(down) related file locations
#define GPIO_DIRECTION_DOWN	 "/sys/class/gpio/gpio46/direction"
#define GPIO_VALUE_DOWN      "/sys/class/gpio/gpio46/value"
#define GPIO_TRIGGER_DOWN    "/sys/class/gpio/gpio46/edge"



/*
Blocks code until joystick is released using epoll.h
input: char**, int
output: int (1 for successful block, -1 for failed blocking)

Code based on
CMPT433 epoll example: https://opencoursehub.cs.sfu.ca/bfraser/solutions/433/guide-code/gpio/edgeTrigger.c
epoll man page: https://man7.org/linux/man-pages/man7/epoll.7.html
*/
int gpioEdgeTrigger(char** fileNameForGpioValues, int size) {
	// Initialize epoll file descriptor
	int epollfd = epoll_create(1);
	
	if (epollfd == -1) {
		return -1;
	}

	// Initialize array of epoll_events
	// epoll_struct: keep track of Interest List of file descriptors
	// events: keep track of ready list of file descriptors
	struct epoll_event events[MAX_EVENTS], epoll_struct[MAX_EVENTS];
	int fd_Array[size];

	// set values for epoll_events (All GPIO joystick file decriptors)
	for(int i = 0; i < size; i++) {
		fd_Array[i] = open(fileNameForGpioValues[i], O_RDONLY | O_NONBLOCK);
		if (fd_Array[i] == -1) {
			return - 1;
		}

		epoll_struct[i].events = EPOLLIN | EPOLLET | EPOLLPRI;
		epoll_struct[i].data.fd = fd_Array[i];

		//Register GPIO joystick file decriptors to epollfd
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_Array[i], &epoll_struct[i]) == -1) {
			return -1;
		}
	}

	// Loop twice in order for blocking to be release until joystick is released
	for (int i = 0; i < 2; i++) {
		// Check for number of file descriptors that are in ready list
		// 1 if joystick is pressed
		// 4 if joystick is released
		int availableFds = epoll_wait(epollfd, events, 4, -1);

		if (availableFds == -1){
			for(int i = 0; i < size; i++) {
				close(fd_Array[i]);
			}
			close(epollfd);
			return -1;
		}

	}

	// close file descriptors and epoll instance
	for(int i = 0; i < size; i++) {
		close(fd_Array[i]);
	}
	close(epollfd);
	return 0;
}



/*
Return the direction of joystick pressed
input: void
output: enum
*/
directions getDirections() {
	// set joystick direction for input
	setFile(GPIO_DIRECTION_LEFT, DIRECTION_SETTING);
	setFile(GPIO_DIRECTION_RIGHT, DIRECTION_SETTING);
	setFile(GPIO_DIRECTION_UP, DIRECTION_SETTING);
	setFile(GPIO_DIRECTION_DOWN, DIRECTION_SETTING);

	// set joystick trigger mode to falling
	setFile(GPIO_TRIGGER_LEFT, EDGE_TRIGGER_SETTING);
	setFile(GPIO_TRIGGER_RIGHT, EDGE_TRIGGER_SETTING);
	setFile(GPIO_TRIGGER_UP, EDGE_TRIGGER_SETTING);
	setFile(GPIO_TRIGGER_DOWN, EDGE_TRIGGER_SETTING);

	// list GPIO file locations used for blocking
	char *gpioFileNames[] = {
	    GPIO_VALUE_LEFT,
	    GPIO_VALUE_RIGHT,
	    GPIO_VALUE_UP,
	    GPIO_VALUE_DOWN
	};

	while (true) {
		// Set blocking
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
