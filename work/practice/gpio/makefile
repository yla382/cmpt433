all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror gpio.c -o gpio
	cp gpio $(HOME)/cmpt433/public/myApps/

clean:
	rm gpio
	rm $(HOME)/cmpt433/public/myApps/gpio