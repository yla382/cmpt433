all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror a2d.c -o a2d
	cp a2d $(HOME)/cmpt433/public/myApps/

clean:
	rm a2d
	rm $(HOME)/cmpt433/public/myApps/a2d