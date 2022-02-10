all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror sleep.c display.c a2d.c circular_buffer.c light_sampler.c -o light_sampler -pthread
	cp light_sampler $(HOME)/cmpt433/public/myApps/

clean:
	rm light_sampler
	rm $(HOME)/cmpt433/public/myApps/light_sampler