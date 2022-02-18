all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror led.c -o led
	cp led $(HOME)/cmpt433/public/myApps/

clean:
	rm led
	rm $(HOME)/cmpt433/public/myApps/led