all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror udp.c -o udp
	cp udp $(HOME)/cmpt433/public/myApps/

clean:
	rm udp
	rm $(HOME)/cmpt433/public/myApps/udp