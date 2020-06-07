#include <signal.h>
#include <unistd.h>
#include <sense/sense.h>
#include <stdlib.h>
#include <stdio.h>

pi_framebuffer_t *fb;
int run = 1;
int red;
int orange;
int yellow;
int green;
int blue;
int purple;
int white;
int blank;

typedef struct Shape{
	int layout[3][3];
	int color;
} Shape; 

enum colors{Red = 1, Orange, Yellow, Green, Blue, Purple, White};

//Rewrites the shape of the activePiece depending on pieceNum (the type of piece)
//Also takes the address of the shape variable as a paremeter, which is assigned to a pointer
void createPiece(int pieceNum, Shape *activePiece){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			activePiece->layout[i][j] = 0;
		}
	}
	
	switch(pieceNum){
		case 1:
			//Straight piece
			activePiece->color = red;
			activePiece->layout[0][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			break;
		case 2:
			//Square
			activePiece->color = orange;
			activePiece->layout[1][0] = 1;
			activePiece->layout[2][0] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			break;
		case 3:
			//T-shape
			activePiece->color = yellow;
			activePiece->layout[2][0] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->layout[2][2] = 1;
			break;
		case 4:
			//Right L-shape
			activePiece->color = green;
			activePiece->layout[0][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->layout[2][2] = 1;
			break;
		case 5:
			//Left L-shape
			activePiece->color = blue;
			activePiece->layout[0][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->layout[2][0] = 1;
			break;
		case 6:
			//Right zigzag
			activePiece->color = purple;
			activePiece->layout[2][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][2] = 1;
			activePiece->layout[0][2] = 1;
			break;
		case 7:
			//Left zigzag
			activePiece->color = white;
			activePiece->layout[2][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][0] = 1;
			activePiece->layout[0][0] = 1;
			break;	
	}
}

//Used to exit the program when needed
void interrupt_handler(int sig){
	run = 0;
	clearBitmap(fb->bitmap, blank);
	exit(0);
}

int main(){
	signal(SIGINT, interrupt_handler);
	fb = getFBDevice();
	clearBitmap(fb->bitmap, blank);
	red = getColor(243, 51, 51);
	orange = getColor(255, 90, 22);
	yellow = getColor(255, 225, 0);
	green = getColor(37, 141, 0);
	blue = getColor(51, 51, 255);
	purple = getColor(121, 35, 169);
	white = getColor(255, 255, 255);
	blank = getColor(0, 0, 0);	

	Shape activePiece;
	
}
