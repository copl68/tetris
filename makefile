tetris: tetris.o libsense_2.0/framebuffer.o libsense_2.0/joystick.o
	make -C libsense_2.0
	gcc tetris.o libsense_2.0/framebuffer.o libsense_2.0/joystick.o -o tetris -lsense
tetris.o: main.c
	gcc -c main.c -o tetris.o
clean: rm -f tetris *.o
