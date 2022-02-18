all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror capture.c -lv4l2 -o capture
	cp capture $(HOME)/cmpt433/public/myApps/

clean:
	rm capture
	rm $(HOME)/cmpt433/public/myApps/capture