#include <signal.h>
#include <unistd.h>
#include <sense/sense.h>
#include <stdlib.h>

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

enum colors{Red, Orange, Yellow, Green, Blue, Purple, White};

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

	
}
