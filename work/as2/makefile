CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
C_FLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
C_FILES = network.c command_string.c sleep.c display.c a2d.c circular_buffer.c light_sampler.c

all:
	$(CC_C) $(C_FLAGS) noworky.c -o noworky
	$(CC_C) $(C_FLAGS) $(C_FILES) -o light_sampler -pthread
	cp light_sampler $(HOME)/cmpt433/public/myApps/
	cp noworky $(HOME)/cmpt433/public/myApps/

clean:
	rm light_sampler
	rm $(HOME)/cmpt433/public/myApps/noworky
	rm $(HOME)/cmpt433/public/myApps/light_sampler