all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror display.c -o display
	cp display $(HOME)/cmpt433/public/myApps/

clean:
	rm display
	rm $(HOME)/cmpt433/public/myApps/display