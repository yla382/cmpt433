all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror hello.c joyStickControl.c ledControl.c fileHandler.c directions.h -o hello
	cp hello $(HOME)/cmpt433/public/myApps/

clean:
	rm hello
	rm $(HOME)/cmpt433/public/myApps/hello