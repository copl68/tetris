tetris: tetris.o 
	gcc tetris.o -o tetris -lsense
tetris.o: main.c
	gcc -c main.c -o tetris.o
clean: 
	rm -f tetris *.o
