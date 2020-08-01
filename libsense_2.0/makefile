all: framebuffer.o joystick.o 
framebuffer.o: framebuffer.c framebuffer.h
	gcc -c framebuffer.c -o framebuffer.o
joystick.o: joystick.c joystick.h
	gcc -c joystick.c -o joystick.o
clean: 
	rm -f libsense_2.0 *.o
