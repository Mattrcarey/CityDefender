#define _DEFAULT_SOURCE
#include<stdlib.h>
#include<pthread.h>
#include<stdio.h>
#include<curses.h>
#include<unistd.h>
#include<string.h>
#include"missile.h"

//@author Matthew Carey

static int maxX; //width of the window
static int maxY; //height of the window
pthread_mutex_t mutex; 


//This function is used to set the global variables for the missiles
//@param x: the width of the window
//@param y: the height of the window
//@param m: the initialised mutex
void init_missiles(int x, int y, pthread_mutex_t m){
	maxX = x;
	maxY = y;
	mutex = m;
}


//This function creates a missile struct
//No parameters because row is always 1 at the start and the column and
//speed are randomised
//@returns a missile struct
Missile* makeMissile(){
	srand(time(NULL));
	Missile* missile = malloc(sizeof(Missile));
	missile->row = 1;
	missile->col = rand()%maxX;
	missile->speed = (((rand()%3)+1)*100000);
	return missile;
}


//Frees a Missile struct
//@param missile: the missile to be freed
void destroyMissile(Missile* missile){
	if(missile!=NULL){
		free(missile);
	}
}


//This function checks for collisions
//@param missile: the missile struct includes the current row and 
//collumn so this function can check the next location
//returns 0 on failure and the character located and the next location
//on a collision
int collition(Missile* missile){
	pthread_mutex_lock(&mutex);
	char ch;
	ch = mvinch(missile->row+1,missile->col);
	if((ch=='_') || (ch == '|') || (ch == '#') || (ch=='*')){
		pthread_mutex_unlock(&mutex);
		return ch;
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}


//this functions runs the missile making it move down checking for collisions 
//at each new coordinate 
//@param missile: the missile struct
void* runMissile(void* missile){
	Missile* m = missile;
	pthread_mutex_lock(&mutex);
	mvprintw(m->row,m->col,"|");
	refresh();
	pthread_mutex_unlock(&mutex);
	while(1){
		if(m->row+2==maxY){//checks if its at the bottom
			pthread_mutex_lock(&mutex);
			mvprintw(m->row,m->col,"?");
			refresh();
			pthread_mutex_unlock(&mutex);	
			break;
		}
		if(collition(m)){//checks for a collision
			break;
		}
		pthread_mutex_lock(&mutex);//with no colliion we move down 1 row
		mvprintw(m->row,m->col," ");
		m->row=m->row+1;
		mvprintw(m->row,m->col,"|");
		refresh();
		pthread_mutex_unlock(&mutex);
		usleep(m->speed);
	}
	if(m->row+2!=maxY){
		if((collition(m)=='#')||(collition(m)=='|')){
			pthread_mutex_lock(&mutex);
			mvprintw(m->row,m->col,"?");
			mvprintw(m->row+1,m->col,"*");
			refresh();
			pthread_mutex_unlock(&mutex);
		}
		else if(m->row!=maxY){
			pthread_mutex_lock(&mutex);
			mvprintw(m->row,m->col," ");
			mvprintw(m->row+2,m->col,"*");
			refresh();
			mvprintw(m->row+1,m->col,"?");
			refresh();
			pthread_mutex_unlock(&mutex);
		}
	}
	destroyMissile(m);//frees itself at the end
	pthread_exit(0);

}



