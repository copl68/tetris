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
int board[8][8];

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

//Draws the board to the framebuffer
void drawBoard(){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			setPixel(fb->bitmap, i, j, board[i][j]);
		}
	}
	//FIXME: only draw the needed part of the board again
}

//Draws activePiece on the board according to where startX and startY want the piece to start
void drawPiece(Shape *activePiece, int changeX, int changeY){
	drawBoard();
	bool willCollide = false;
	//Check to see if the piece will collide with an occupied space
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if((startX + i + changeX) < 8 && (startX + i + changeX) > -1 && (startY + j + changeY) < 8 && (startY + j + changeY) > -1){
				if(piece.layout[i][j] == 1 && board[startX + i + changeX][startY + j + changeY] != blank){
					willCollide = true;
				}				
			}
		}
	}
	//Clears where the piece previously was
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			//Checks to see if pixel is in framebuffer range
			if((startX + i) < 8 && (startX + i) > -1 && (startY + j) < 8 && (startY + j) > -1){
				if(piece.layout[i][j] == 1){
					setPixel(fb->bitmap, startX + i, startY + j, blank);
				}
			}
		}
	}
	//printf("x before: %d\n", startX);
	//printf("y before: %d\n", startY);
	if(!willCollide){
		startX = startX + changeX;
		startY = startY + changeY;
	}	
	//printf("x after: %d\n", startX);
	//printf("y after: %d\n", startY);
	//Prints the pieces new position to the screen
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

//Rotates the piece 90 degreees clockwise when called. This updates the piece layout and which sides are empty
void rotatePiece(){
	//Don't rotate square piece
	if(piece.code == 2){ return;}
	//Check if a rotated piece will collide with an occupied space
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			printf("i: %d\n", i);
			printf("j: %d\n", j);
			printf("startX: %d\n", startX);
			printf("startY: %d\n\n", startY);
			if((piece.code == 3 || piece.code == 6 || piece.code == 7) && piece.leftEmpty){
				if(piece.layout[j][2-i+1] == 1 && board[startX + i][startY + j - 1] != blank){
					printf("first return\n");
					return;
				}
			}
			else if((startX + i > -1) && (startX + i < 8) && (startY + j > -1) && (startY + j < 8)){
				if(piece.layout[j][2-i] == 1 && board[startX + i][startY + j] != blank){
					printf("Got here...\n");
					return;
				}
		
			}
		}
		//FIXME: THIS ISNT WORKING FOR PURPLE AT LEAST. JUST FIX ROTATION COLLISION!!!
	}
	int layoutCopy[3][3];
	//Create copy of layout
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			layoutCopy[i][j] = piece.layout[i][j];
		}
	}

	//Update the layout array with an rotated version
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			piece.layout[i][j] = layoutCopy[j][2-i];
		}
	}

	//Rotating a t-shape or zigzag when the left is empty will put in on the bottom of the array. This shifts the shape to the top
	//of the layout array so that the player has more time to move the piece and think
	if((piece.code == 3 || piece.code == 6 || piece.code == 7) && piece.leftEmpty){
		for(int i = 0; i < 3; i++){
			piece.layout[0][i] = piece.layout[1][i];
			piece.layout[1][i] = piece.layout[2][i];
			piece.layout[2][i] = 0;
		}
	}

	//Makes sure that the piece is drawn on the screen even if the layout array is partially off screen
	if(startX == -1 && !piece.leftEmpty){
		drawPiece(&piece, 1, 0);
	}
	else if(startX == 6 && !piece.rightEmpty){
		drawPiece(&piece, -1, 0);
	}
	else if(startY == -1 && !piece.topEmpty){
		drawPiece(&piece, 0, 1);
	}
	else if(startY == 6 && !piece.bottomEmpty){
		drawPiece(&piece, 0, -1);
	}
	else{
		drawPiece(&piece, 0, 0);
	}

	//Updates which sides of the layout array are empty after rotation
	if(piece.topEmpty){
		piece.topEmpty = false;
		piece.rightEmpty = true;
		if(piece.code == 1){
			piece.leftEmpty = true;
			piece.bottomEmpty = false;
		}
	}
	else if(piece.leftEmpty){
		piece.leftEmpty = false;
		piece.topEmpty = true;
		if(piece.code == 1){
			piece.bottomEmpty = true;
			piece.rightEmpty = false;
		}
		else if(piece.code == 3 || piece.code == 6 || piece.code == 7){
			//Since we shifted these pieces to the top of the layout array
			piece.topEmpty = false;
			piece.bottomEmpty = true;
		}	
	}
	else if(piece.bottomEmpty){
		piece.bottomEmpty = false;
		piece.leftEmpty = true;
	}
	else if(piece.rightEmpty){
		piece.rightEmpty = false;
		piece.bottomEmpty = true;
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
			rotatePiece();
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
	
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			board[i][j] = blank;	
		}
	}

	board[1][1] = purple;
	drawBoard();

	pi_joystick_t* joystick = getJoystickDevice();
	startX = 2;
	startY = 2;
	srand(time(0));
	createPiece(rand() % 8 + 1, &piece);
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			printf(" %d ", piece.layout[i][j]);
		}
		printf("\n");
	}
	drawPiece(&piece, 0, 0);
	while(run){
		pollJoystick(joystick, movePiece, 1000);
	}
}
