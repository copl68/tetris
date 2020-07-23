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
int points;

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
void createPiece(int pieceNum){
	printf("createPiece\n");
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			piece.layout[i][j] = 0;
		}
	}
	
	piece.code = pieceNum;
	piece.topEmpty = false;
	piece.bottomEmpty = false;
	piece.rightEmpty = false;
	piece.leftEmpty = false;
	switch(pieceNum){
		case 1:
			//Straight piece
			piece.color = red;
			piece.layout[0][1] = 1;
			piece.layout[1][1] = 1;
			piece.layout[2][1] = 1;
			piece.rightEmpty = true;
			piece.leftEmpty = true;
			break;
		case 2:
			//Square
			piece.color = orange;
			piece.layout[1][0] = 1;
			piece.layout[0][0] = 1;
			piece.layout[1][1] = 1;
			piece.layout[0][1] = 1;
			piece.bottomEmpty = true;
			piece.rightEmpty = true;
			break;
		case 3:
			//T-shape
			piece.color = yellow;
			piece.layout[0][0] = 1;
			piece.layout[1][1] = 1;
			piece.layout[0][1] = 1;
			piece.layout[0][2] = 1;
			piece.bottomEmpty = true;
			break;
		case 4:
			//Left L-shape
			piece.color = green;
			piece.layout[0][1] = 1;
			piece.layout[1][1] = 1;
			piece.layout[2][1] = 1;
			piece.layout[2][0] = 1;
			piece.rightEmpty = true;
			break;
		case 5:
			//Right L-shape
			piece.color = blue;
			piece.layout[0][0] = 1;
			piece.layout[1][0] = 1;
			piece.layout[2][0] = 1;
			piece.layout[2][1] = 1;
			piece.rightEmpty = true;
			break;
		case 6:
			//Left zigzag
			piece.color = purple;
			piece.layout[2][1] = 1;
			piece.layout[1][1] = 1;
			piece.layout[1][0] = 1;
			piece.layout[0][0] = 1;
			piece.rightEmpty = true;
			break;
		case 7:
			//Right zigzag
			piece.color = white;
			piece.layout[2][0] = 1;
			piece.layout[1][0] = 1;
			piece.layout[1][1] = 1;
			piece.layout[0][1] = 1;
			piece.rightEmpty = true;
			break;	
	}
}

//Draws the board to the framebuffer
void drawBoard(){
	printf("drawBoard\n");
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			setPixel(fb->bitmap, i, j, board[i][j]);
		}
	}
}

//Draws activePiece on the board according to where startX and startY want the piece to start
void drawPiece(Shape *activePiece, int changeX, int changeY){
	printf("drawPiece\n");
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

//Loops through to check if a piece is colliding with an occupied space on the board
bool checkCollision(int xOffset, int yOffset){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(piece.layout[i][j] == 1 && board[startX + i + xOffset][startY + j + yOffset] != blank){
				return true;
			}
		}
	}
	return false;
}

//Acts as an "undo" feature when a piece has been rotated when it shouldn't have been
void unrotate(int layoutCopy[3][3]){
	for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++){
				piece.layout[i][j] = layoutCopy[i][j];
			}	
		}
}

//Rotates the piece 90 degreees clockwise when called. This updates the piece layout and which sides are empty
void rotatePiece(){
	//Don't rotate square piece
	if(piece.code == 2){ return;}
	
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

	//Rotating a t-shape or zigzag when the right is empty will put in on the bottom of the array. This shifts the shape to the top
	//of the layout array so that the player has more time to move the piece and think
	if(!(piece.code == 1 || piece.code == 2) && piece.rightEmpty){
		for(int i = 0; i < 3; i++){
			piece.layout[0][i] = piece.layout[1][i];
			piece.layout[1][i] = piece.layout[2][i];
			piece.layout[2][i] = 0;
		}
	}

	//Checks to see if a pieces rotation will collide with an occupied board space
	bool undo = checkCollision(0, 0);

	//Undo the rotation if a collision will occur
	if(undo){
		unrotate(layoutCopy);
		return;
	}

	//Makes sure that the piece is drawn on the screen even if the layout array is partially off screen
	if(startX == -1 && !piece.rightEmpty){
		if(checkCollision(1, 0)){
			unrotate(layoutCopy);
			return;
		}
		else{
			drawPiece(&piece, 1, 0);
		}
	}
	else if(startX == 6 && !piece.leftEmpty){
		if(checkCollision(-1, 0)){
			unrotate(layoutCopy);
			return;
		}
		else{
			drawPiece(&piece, -1, 0);
		}
	}
	else if(startY == -1 && !piece.topEmpty){
		if(checkCollision(0, 1)){
			unrotate(layoutCopy);
			return;
		}
		else{
			drawPiece(&piece, 0, 1);
		}
	}
	else if(startY == 6 && !piece.bottomEmpty){
		if(checkCollision(0, -1)){
			unrotate(layoutCopy);
			return;
		}	
		else{
			drawPiece(&piece, 0, -1);
		}
	}
	else{
		drawPiece(&piece, 0, 0);
	}

	//Updates which sides of the layout array are empty after rotation
	if(piece.topEmpty){
		piece.topEmpty = false;
		piece.leftEmpty = true;
		if(piece.code == 1){
			piece.rightEmpty = true;
			piece.bottomEmpty = false;
		}
	}
	else if(piece.leftEmpty){
		piece.leftEmpty = false;
		piece.bottomEmpty = true;
		if(piece.code == 1){
			piece.topEmpty = true;
			piece.rightEmpty = false;
		}
	}
	else if(piece.bottomEmpty){
		piece.bottomEmpty = false;
		piece.rightEmpty = true;
	}
	else if(piece.rightEmpty){
		piece.rightEmpty = false;
		piece.topEmpty = true;
		if(!(piece.code == 1 || piece.code == 2)){
			//Since we shifted these pieces to the top of the layout
			piece.topEmpty = false;
			piece.bottomEmpty = true;
		}
	}
}	

//Moves the piece up, down, left, or right when the joystick is pressed
void movePiece(unsigned int code){
	switch(code){
		case KEY_UP:
			//Checks if pieces layout array will be in bounds
			if(startY < 5){		
				drawPiece(&piece, 0, 1);
			}
			//if the layout array will go out of bounds, it must not contain any pixels of the actual piece
			else if(startY == 5 && piece.rightEmpty){
				drawPiece(&piece, 0, 1);
			}
			break;
		case KEY_DOWN:
			if(startY > 0){
				drawPiece(&piece, 0, -1);
			}
			else if(startY == 0 && piece.leftEmpty){
				drawPiece(&piece, 0, -1);
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

//Checks to see if the piece can move down another space
bool spaceBelow(){
	printf("spaceBelow\n");
	//If the array where the piece is is empty on the bottom, you need to check one row above where you normally would. 
	int empty = piece.bottomEmpty ? -1 : 0;
	bool space = true;
	//If there is an occupied space below the shape, it cant move down 
	int upAnother;
	printf("\n\n");
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			printf(" %d ", piece.layout[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for(int i = 0; i < 8; i ++){
		for(int j = 0; j < 8; j++){
			if(board[i][j] != blank){
				printf(" 1 ");
			}
			else{
				printf(" 0 ");
			}
		}
		printf("\n");
	}
	printf("\n");
	for(int i = 0; i < 3; i++){
		upAnother = piece.layout[2 + empty][i] != blank ? 0 : -1;	//If theres not a piece on the bottom of the shape array being looked at
		//If a block right under a piece is occupied and a piece occupies a space right above that block
		printf(" %d %d \n", board[startX + 3 + empty + upAnother][startY + i] != blank, piece.layout[2 + empty + upAnother][i] != blank);
		if(board[startX + 3 + empty + upAnother][startY + i] != blank && piece.layout[2 + empty + upAnother][i] != blank){
			printf("\n\nno room - %d\n\n", i);
			space = false;
		}
	}
	if((startX + empty) == 5){ space = false;}
	return space;
}

//Simulates gravity to condense the blocks so that there are no empty rows. 
void condense(int fullRows[8]){
	int fullCount = 0;
	for(int row = fullRows[fullCount]; row > -1; row--){
		if(row == fullRows[fullCount]){
			fullCount++;
		}
		else{
			for(int col = 0; col < 8; col++){
				board[row + fullCount][col] = board[row][col];
				board[row][col] = blank;
			}
		}
	}
}

//Checks to see if any rows on the board are full. If so, clear them
void checkRows(){
	bool full = true;
	int fullRows[8] = {-1,-1,-1,-1,-1,-1,-1,-1};	//Keep track of which rows are full
	int fullCount = 0;
	
	for(int row = 7; row > -1; row--){
		for(int col = 0; col < 8; col++){
			if(board[row][col] == blank){
				full = false;
				col = 7;
			}
		}
		if(full){
			fullRows[fullCount] = row;
			fullCount++;	
		}
		full = true;
	}

	//Calculating points based off of how many lines are cleared at once (tetris.wiki/Scoring)
	switch(fullCount){
		case 1: 
			points += 40;
			break;
		case 2: 
			points += 100;
			break;
		case 3: 
			points += 300;
			break;
		default: 
			break;
	}

	fullCount = 0;
	if(fullRows[0] != -1){
		//Quick animation to  clear rows
		for(int distOut = 0; distOut < 4; distOut++){
			while(fullRows[fullCount] != -1){
				setPixel(fb->bitmap, fullRows[fullCount], 3 - distOut, white);
				setPixel(fb->bitmap, fullRows[fullCount], 4 + distOut, white);
				fullCount++;
			}
			fullCount = 0;
			usleep(100000);
		}

		while(fullRows[fullCount] != -1){
			for(int col = 0; col < 8; col++){
				board[fullRows[fullCount]][col] = blank;
				setPixel(fb->bitmap, fullRows[fullCount], col, blank);
			}
			fullCount++;
		}
		condense(fullRows);
	}
	return;
}

bool canUsePiece(){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(board[startX + i][startY + j] != blank && piece.layout[i][j] == 1){
				return false;
			}
		}
	}
	return true;
}

//Used to exit the program when needed
void interrupt_handler(int sig){
	run = 0;
	printf("\n       GAME OVER\n\n*** POINTS SCORED: %d ***\n\n", points);
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
	time_t rawtime;
	time_t prevtime;
	points = 0;

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			board[i][j] = blank;	
		}
	}

	drawBoard();

	pi_joystick_t* joystick = getJoystickDevice();
	startX = 0;
	startY = 3;
	srand(time(0));
	createPiece(rand() % 7 + 1);

	drawPiece(&piece, 0, 0);
	time(&rawtime);
	time(&prevtime);
	while(run){
		//Moves the piece down once a second has gone by
		if((int)difftime(rawtime, prevtime) != 0){
			if(spaceBelow()){
				printf("Theres space below!!\n");
				drawPiece(&piece, 1, 0);
			}
			else{
				for(int i = 0; i < 3; i++){
					for(int j = 0; j < 3; j++){
						if(piece.layout[i][j] != blank){
							board[startX + i][startY + j] = piece.color;
						}
					}
				}	
				checkRows();
				int r = rand() % 7 + 1;
				printf("%d\n", r);
				createPiece(r);
				startX = 0; 
				startY = 3;
				if(canUsePiece()){
					drawPiece(&piece, 0, 0);
				}
				else{
					sleep(1);
					interrupt_handler(0);
				}
			}
		}
		prevtime = rawtime;
		time(&rawtime);
		pollJoystick(joystick, movePiece, 10);
	}
}
