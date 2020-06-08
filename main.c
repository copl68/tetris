#include <signal.h>
#include <unistd.h>
#include <sense/sense.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/input.h>
#include <stdbool.h>
#include <time.h>

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
	bool topEmpty; 
	bool bottomEmpty;
	bool rightEmpty;
	bool leftEmpty;
} Shape; 

Shape piece;

//Rewrites the shape of the activePiece depending on pieceNum (the type of piece)
//Also takes the address of the shape variable as a paremeter, which is assigned to a pointer
void createPiece(int pieceNum, Shape *activePiece){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			activePiece->layout[i][j] = 0;
		}
	}
	
	activePiece->code = pieceNum;
	activePiece->topEmpty = false;
	activePiece->bottomEmpty = false;
	activePiece->rightEmpty = false;
	activePiece->leftEmpty = false;
	switch(pieceNum){
		case 1:
			//Straight piece
			activePiece->color = red;
			activePiece->layout[0][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->rightEmpty = true;
			activePiece->leftEmpty = true;
			break;
		case 2:
			//Square
			activePiece->color = orange;
			activePiece->layout[1][2] = 1;
			activePiece->layout[2][2] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->topEmpty = true;
			activePiece->rightEmpty = true;
			break;
		case 3:
			//T-shape
			activePiece->color = yellow;
			activePiece->layout[2][0] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->layout[2][2] = 1;
			activePiece->topEmpty = true;
			break;
		case 4:
			//Left L-shape
			activePiece->color = green;
			activePiece->layout[0][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->layout[2][2] = 1;
			activePiece->rightEmpty = true;
			break;
		case 5:
			//Right L-shape
			activePiece->color = blue;
			activePiece->layout[0][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[2][1] = 1;
			activePiece->layout[2][0] = 1;
			activePiece->leftEmpty = true;
			break;
		case 6:
			//Left zigzag
			activePiece->color = purple;
			activePiece->layout[2][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][2] = 1;
			activePiece->layout[0][2] = 1;
			activePiece->rightEmpty = true;
			break;
		case 7:
			//Right zigzag
			activePiece->color = white;
			activePiece->layout[2][1] = 1;
			activePiece->layout[1][1] = 1;
			activePiece->layout[1][0] = 1;
			activePiece->layout[0][0] = 1;
			activePiece->leftEmpty = true;
			break;	
	}
}

//Draws activePiece on the board according to where startX and startY want the piece to start
void drawPiece(Shape *activePiece, int changeX, int changeY){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			//Checks to see if pixel is in framebuffer range
			if((startX + i) < 8 && (startX + i) > -1 && (startY + j) < 8 && (startY + j) > -1){
				setPixel(fb->bitmap, startX + i, startY + j, blank);
			}
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
			//Checks to see if pixel is in framebuffer range
			if((startX + i) < 8 && (startX + i) > -1 && (startY + j) < 8 && (startY + j) > -1){
				if(activePiece->layout[i][j] == 1){
					setPixel(fb->bitmap, startX + i, startY + j, activePiece->color);
				}
				//else{
				//	setPixel(fb->bitmap, startX + i, startY + j, white);
				//}	
			}
		}
	}
}

//Moves the piece up, down, left, or right when the joystick is pressed
void movePiece(unsigned int code){
	switch(code){
		case KEY_UP:
			//Checks if pieces layout array will be in bounds
			if(startY > 0){		
				drawPiece(&piece, 0, -1);
			}
			//if the layout array will go out of bounds, it must not contain any pixels of the actual piece
			else if(startY == 0 && piece.rightEmpty){
				drawPiece(&piece, 0, -1);
			}
			break;
		case KEY_DOWN:
			if(startY < 5){
				drawPiece(&piece, 0, 1);
			}
			else if(startY == 5 && piece.leftEmpty){
				drawPiece(&piece, 0, 1);
			}
			break;
		case KEY_RIGHT:
			if(startX < 5){
				drawPiece(&piece, 1, 0);
			}
			else if(startX == 5 && piece.bottomEmpty){
				drawPiece(&piece, 1, 0);
			}
			break;
		case KEY_LEFT:
			if(startX > 0){
				drawPiece(&piece, -1, 0);
			}
			else if(startX == 0 && piece.topEmpty){
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
	srand(time(0));
	createPiece(rand() % 8 + 1, &piece);
	drawPiece(&piece, 0, 0);
	while(run){
		pollJoystick(joystick, movePiece, 1000);
	}
}
