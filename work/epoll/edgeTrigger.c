// Edge trigger the joystic left
//
// Based on code by Derek Malloy: http://exploringbeaglebone.com/chapter6/
// Originally based on code in epoll man page
// Modified by Brian Fraser

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>		// Errors
#include <string.h>

#include <sys/epoll.h>  // for epoll()
#include <fcntl.h>      // for open()
#include <unistd.h>     // for close()

#define MAX_EVENTS 4
#define JOYSTICK_DIRECTION_LEFT "/sys/class/gpio/gpio65/direction"
#define JOYSTICK_EDGE_LEFT      "/sys/class/gpio/gpio65/edge"
#define JOYSTICK_IN_LEFT        "/sys/class/gpio/gpio65/value"

#define JOYSTICK_DIRECTION_RIGHT "/sys/class/gpio/gpio47/direction"
#define JOYSTICK_EDGE_RIGHT      "/sys/class/gpio/gpio47/edge"
#define JOYSTICK_IN_RIGHT        "/sys/class/gpio/gpio47/value"

#define JOYSTICK_DIRECTION_UP 	"/sys/class/gpio/gpio26/direction"
#define JOYSTICK_EDGE_UP		"/sys/class/gpio/gpio26/edge"
#define JOYSTICK_IN_UP        	"/sys/class/gpio/gpio26/value"

#define JOYSTICK_DIRECTION_DOWN	"/sys/class/gpio/gpio46/direction"
#define JOYSTICK_EDGE_DOWN		"/sys/class/gpio/gpio46/edge"
#define JOYSTICK_IN_DOWN       	"/sys/class/gpio/gpio46/value"


static void writeToFile(const char* fileName, const char* value);
static int readLineFromFile(char* fileName, char* buff, unsigned int maxLength);


static int waitForGpioEdge(const char** fileNameForGpioValues, const int size) {
	// If you want to wait for input on multiple file, you could change this function
	// to take an array of names, and then loop throug each, adding it to the 
	// epoll instance (accessed via epollfd) by calling epoll_ctl() numerous times.

	// create an epoll instance
	// .. kernel ignores size argument; must be > 0

	struct epoll_event events[MAX_EVENTS], epoll_struct[MAX_EVENTS];
	int fds[size];

	int epollfd = epoll_create(1);
	if (epollfd == -1) {
		fprintf(stderr, "ERROR: epoll_create() returned (%d) = %s\n", epollfd, strerror(errno));
		return -1;
	}

	for(int i = 0; i < size; i++) {
		fds[i] = open(fileNameForGpioValues[i], O_RDONLY | O_NONBLOCK);
		if (fds[i] == -1) {
			fprintf(stderr, "ERROR: unable to open() %s = %s\n", fileNameForGpioValues[i], strerror(errno));
			return -1;
		}

		// configure epoll to wait for events: read operation | edge triggered | urgent data
		epoll_struct[i].events = EPOLLIN | EPOLLET | EPOLLPRI;
		epoll_struct[i].data.fd = fds[i];

		// register file descriptor on the epoll instance, see: man epoll_ctl
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fds[i], &epoll_struct[i]) == -1) {
			fprintf(stderr, "ERROR: epoll_ctl() failed on add control interface (%s) = %s\n", fileNameForGpioValues[i], strerror(errno));
			return -1;
		}
	}


	// ignore first trigger
	for (int i = 0; i < 2; i++) {
		int waitRet = epoll_wait(epollfd, events, 4, -1);

		printf("%d\n", waitRet);
		if (waitRet == -1){
			fprintf(stderr, "ERROR: epoll_wait() failed (%d) = %s\n", waitRet, strerror(errno));
			for(int i = 0; i < size; i++) {
				close(fds[i]);
			}
			close(epollfd);
			return -1;
		}

	}

	// cleanup epoll instance:
	for(int i = 0; i < size; i++) {
		close(fds[i]);
	}

	close(epollfd);
	return 0;
}

int main() 
{
	// Assume joystick pin already exported as GPIO
	// .. set direction of GPIO pin to input
	writeToFile(JOYSTICK_DIRECTION_LEFT, "in");
    writeToFile(JOYSTICK_DIRECTION_RIGHT, "in");
    writeToFile(JOYSTICK_DIRECTION_UP, "in");
    writeToFile(JOYSTICK_DIRECTION_DOWN, "in");

	// .. set edge trigger; options are "none", "rising", "falling", "both"
	writeToFile(JOYSTICK_EDGE_LEFT, "falling");
    writeToFile(JOYSTICK_EDGE_RIGHT, "falling");
    writeToFile(JOYSTICK_EDGE_UP, "falling");
    writeToFile(JOYSTICK_EDGE_DOWN, "falling");

	// Block and wait for edge triggered change on GPIO pin
	printf("Now waiting on input on joystick\n");


    const char * gpioFileNames[] = {
	    JOYSTICK_IN_LEFT,
	    JOYSTICK_IN_RIGHT,
	    JOYSTICK_IN_UP,
	    JOYSTICK_IN_DOWN
	};

	while (1) {

		// Wait for an edge trigger:
		int ret1 = waitForGpioEdge(gpioFileNames, 4);
		if (ret1 == -1) {
			exit(EXIT_FAILURE);
		}
	
		char buff1[1024];
		int bytesRead1 = readLineFromFile(JOYSTICK_IN_LEFT, buff1, 1024);
		if (bytesRead1 > 0) {
			printf("GPIO pin reads(LEFT): %c\n", buff1[0]);
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}

        char buff2[1024];
		int bytesRead2 = readLineFromFile(JOYSTICK_IN_RIGHT, buff2, 1024);
		if (bytesRead2 > 0) {
			printf("GPIO pin reads(RIGHT): %c\n", buff2[0]);
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}

		char buff3[1024];
		int bytesRead3 = readLineFromFile(JOYSTICK_IN_UP, buff3, 1024);
		if (bytesRead3 > 0) {
			printf("GPIO pin reads(UP): %c\n", buff3[0]);
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}

        char buff4[1024];
		int bytesRead4 = readLineFromFile(JOYSTICK_IN_DOWN, buff4, 1024);
		if (bytesRead4 > 0) {
			printf("GPIO pin reads(DOWN): %c\n", buff4[0]);
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}

		printf("\n");
	}
	return 0;
}

static void writeToFile(const char* fileName, const char* value)
{
	FILE *pFile = fopen(fileName, "w");
	fprintf(pFile, "%s", value);
	fclose(pFile);
}

static int readLineFromFile(char* fileName, char* buff, unsigned int maxLength)
{
	FILE *file = fopen(fileName, "r");
	int bytes_read = getline(&buff, &maxLength, file);
	fclose(file);
	return bytes_read;
}
