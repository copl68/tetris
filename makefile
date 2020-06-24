tetris: tetris.o framebuffer.o joystick.o
	gcc tetris.o framebuffer.o joystick.o -o tetris -lsense
tetris.o: main.c
	gcc -c main.c -o tetris.o
framebuffer.o: framebuffer.c framebuffer.h
	gcc -c framebuffer.c -o framebuffer.o
joystick.o: joystick.c joystick.h
	gcc -c joystick.c -o joystick.o
clean: 
	rm -f tetris *.o
