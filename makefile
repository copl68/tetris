tetris: tetris.o libsense_2.0/framebuffer.o libsense_2.0/joystick.o
	gcc tetris.o libsense_2.0/framebuffer.o libsense_2.0/joystick.o -o tetris -lsense
tetris.o: main.c
	gcc -c main.c -o tetris.o
libsense_2.0/framebuffer.o: libsense_2.0/framebuffer.c libsense_2.0/framebuffer.h
	gcc -c libsense_2.0/framebuffer.c -o libsense_2.0/framebuffer.o
libsense_2.0/joystick.o: libsense_2.0/joystick.c libsense_2.0/joystick.h
	gcc -c libsense_2.0/joystick.c -o libsense_2.0/joystick.o
clean: 
	rm -f tetris *.o
