#include "sleep.h"

void sleepNow(long seconds, long nanoseconds) {
	struct timespec delayReq = {seconds, nanoseconds};
	nanosleep(&delayReq, (struct timespec *) NULL);
}