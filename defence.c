#define _DEFAULT_SOURCE
#include<stdlib.h>
#include<pthread.h>
#include<stdio.h>
#include<curses.h>
#include<unistd.h>
#include"defence.h"
#include<string.h>


static int maxX;
static int maxY;
static int gameOn = 1;
char** display;
pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER;



void init_defence(int x, int y, char** d, pthread_mutex_t m){
	maxX = x;
	maxY = y;
	display = d;
	mutex = m;
}


Defence* make_defence(int r, int c){
	Defence *defence = malloc(sizeof(Defence));
	defence->row = r;
	defence->col = c;
	defence->graphic = malloc(5);
	strcpy(defence->graphic,"#####");
	return defence;
}

void destroy_defence(Defence* defence){
	if(defence!=NULL){
		free(defence->graphic);
		free(defence);
	}
}

void gameOff(){
	gameOn = 0;
}


void *run(void* d){
	//wait for user input and move
	Defence* defence = d;
	mvprintw(defence->row,defence->col,defence->graphic);
	for(int i=0;i<5;i++){
		display[defence->row][defence->col+i]='#';
	}
	refresh();
	int c = 0;
	while(gameOn){
		c=0;
		switch((c=getch())){
			case 67:
				//move right
				if(defence->col<maxX-5){
					pthread_mutex_lock(&mutex);
					display[defence->row][defence->col] = ' ';
					display[defence->row][defence->col+5] = '#';
					defence->col=defence->col+1;
					move(defence->row,0);
					clrtoeol();
					mvprintw(defence->row,defence->col,defence->graphic);
					refresh();
					pthread_mutex_unlock(&mutex);
				}
				break;
			case 68:
				//move left
				if(defence->col!=0){
					pthread_mutex_lock(&mutex);
					display[defence->row][defence->col+4] = ' ';
					display[defence->row][defence->col-1] = '#';
					move(defence->row,0);
					clrtoeol();
					defence->col=defence->col-1;
                                	mvprintw(defence->row,defence->col,defence->graphic);
					refresh();
					pthread_mutex_unlock(&mutex);
				}
				break;
			default:
				break;
		}			
	}
	pthread_exit(0);

}
