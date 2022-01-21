CC_C = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=200809L -Werror

app:
	$(CC_C) $(CFLAGS) gpio.c -o gpio

clean:
	rm gpio