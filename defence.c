#define _DEFAULT_SOURCE
#include<stdlib.h>
#include<pthread.h>
#include<stdio.h>
#include<curses.h>
#include<unistd.h>
#include"defence.h"
#include<string.h>

//@author Matthew Carey

static int maxX; // the width of the window
static int maxY; // the height of the window
int gameOn = 1; // 1 while the game is still running
pthread_mutex_t mutex; 


//This function sets the global variables for the defence
//@param x: the width of the window
//@param y: the height of the window
//@param m: the initialised mutex
void init_defence(int x, int y, pthread_mutex_t m){
	maxX = x;
	maxY = y;
	mutex = m;
}


//This function creates a defence struct
//@param r: the row of the defence is one above the tallest building
//@param c: the column of the defence is the middle of the window
//@returns the defence struct
Defence* make_defence(int r, int c){
	Defence *defence = malloc(sizeof(Defence));
	defence->row = r;
	defence->col = c;
	defence->graphic = strdup("#####");
	return defence;
}


//This function frees the defence struct
//@param defence: the defence struct
void destroy_defence(Defence* defence){
	if(defence!=NULL){
		free(defence->graphic);
		free(defence);
	}
}


//This function sets gameOn to 0
void gameOff(){
	gameOn = 0;
}


//This function runs the defence thread until gameOn is set to 0
//@param d: the defence struct
void *run(void* d){
	Defence* defence = d;
	pthread_mutex_lock(&mutex);
	mvprintw(defence->row,defence->col,defence->graphic);
	refresh();
	pthread_mutex_unlock(&mutex);
	int c = 0;
	while(1){
		c=0;
		switch((c=getchar())){
			case 67://move right
				if(defence->col<maxX-5){
					pthread_mutex_lock(&mutex);
					defence->col=defence->col+1;
					move(defence->row,0);
					clrtoeol();
					mvprintw(defence->row,defence->col,defence->graphic);
					refresh();
					pthread_mutex_unlock(&mutex);
				}
				break;
			case 68://move left
				if(defence->col!=0){
					pthread_mutex_lock(&mutex);
					move(defence->row,0);
					clrtoeol();
					defence->col=defence->col-1;
                                	mvprintw(defence->row,defence->col,defence->graphic);
					refresh();
					pthread_mutex_unlock(&mutex);
				}
				break;
			case 'q'://if gameOn is 0 then q ends the thread
				if(!gameOn){
					pthread_exit(0);
					break;
				}
			default:
				break;
		}			
	}
	pthread_exit(0);

}
