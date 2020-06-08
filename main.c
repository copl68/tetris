#include <signal.h>
#include <unistd.h>
#include <sense/sense.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/input.h>

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

int startX;
int startY;

typedef struct Shape{
	int layout[3][3];
	int color;
	int code;
} Shape; 

Shape piece;

enum colors{Red = 1, Orange, Yellow, Green, Blue, Purple, White};

//Rewrites the shape of the activePiece depending on pieceNum (the type of piece)
//Also takes the address of the shape variable as a paremeter, which is assigned to a pointer
void createPiece(int pieceNum, Shape *activePiece){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			activePiece->layout[i][j] = 0;
		}
	}
	
	activePiece->code = pieceNum;
	switch(pieceNum){
		case 1:
			//Straight piece
			activePiece->color = red;
			activePiece->layout[1][0] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][2] = 1;
			break;
		case 2:
			//Square
			activePiece->color = orange;
			activePiece->layout[0][1] = 1;
			activePiece->layout[0][2] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][2] = 1;
			break;
		case 3:
			//T-shape
			activePiece->color = yellow;
			activePiece->layout[0][2] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][2] = 1;
			activePiece->layout[2][2] = 1;
			break;
		case 4:
			//Right L-shape
			activePiece->color = green;
			activePiece->layout[1][0] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][2] = 1;
			activePiece->layout[2][2] = 1;
			break;
		case 5:
			//Left L-shape
			activePiece->color = blue;
			activePiece->layout[1][0] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][2] = 1;
			activePiece->layout[0][2] = 1;
			break;
		case 6:
			//Right zigzag
			activePiece->color = purple;
			activePiece->layout[1][2] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->layout[2][0] = 1;
			break;
		case 7:
			//Left zigzag
			activePiece->color = white;
			activePiece->layout[1][2] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[0][1] = 1;
			activePiece->layout[0][0] = 1;
			break;	
	}
}

//Draws activePiece on the board according to where startX and startY want the piece to start
void drawPiece(Shape *activePiece, int changeX, int changeY){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			setPixel(fb->bitmap, startX + i, startY + j, blank);
		}
	}
	//printf("x before: %d\n", startX);
	//printf("y before: %d\n", startY);
	startX = startX + changeX;
	startY = startY + changeY;	
	//printf("x after: %d\n", startX);
	//printf("y after: %d\n", startY);
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(activePiece->layout[i][j] == 1){
				setPixel(fb->bitmap, startX + i, startY + j, activePiece->color);
			}	
		}
	}
}

void movePiece(unsigned int code){
	switch(code){
		case KEY_UP:
			if(startY != 0){		
				drawPiece(&piece, 0, -1);
			}
			break;
		case KEY_DOWN:
			if(startY != 5){
				drawPiece(&piece, 0, 1);
			}
			break;
		case KEY_RIGHT:
			if(startX != 5){
				drawPiece(&piece, 1, 0);
			}
			break;
		case KEY_LEFT:
			if(startX != 0){
				drawPiece(&piece, -1, 0);
			}
			break;
		default:
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

	pi_joystick_t* joystick = getJoystickDevice();
	startX = 2;
	startY = 2;
	createPiece(5, &piece);
	drawPiece(&piece, 0, 0);
	while(run){
		pollJoystick(joystick, movePiece, 1000);
	}	
}
