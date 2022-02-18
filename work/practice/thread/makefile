all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror thread.c -o thread -pthread
	cp thread $(HOME)/cmpt433/public/myApps/

clean:
	rm thread
	rm $(HOME)/cmpt433/public/myApps/thread