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
			activePiece->layout[0][0] = 1;
			activePiece->layout[0][1] = 1;
			activePiece->layout[0][2] = 1;
			break;
		case 2:
			activePiece->color = orange;
			break;
		case 3:
			activePiece->color = yellow;
			break;
		case 4:
			activePiece->color = green;
			break;
		case 5:
			activePiece->color = blue;
			break;
		case 6:
			activePiece->color = purple;
			break;
		case 7:
			activePiece->color = white;
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
	printf("&activePiece: %d\n", &activePiece);
	createPiece(1, &activePiece);	
}
