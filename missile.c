#define _DEFAULT_SOURCE
#include<stdlib.h>
#include<pthread.h>
#include<stdio.h>
#include<curses.h>
#include<unistd.h>
#include<string.h>
#include"missile.h"

static int maxX;
static int maxY;
char** display;
pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER;


void init_missiles(int x, int y, char** d, pthread_mutex_t m){
	maxX = x;
	maxY = y;
	//char** display=malloc(y+1);
	//for(int i = 0; i<y;i++){
        //        display[i] = malloc(x);
                //for(int d=0; d<x;d++){
                        //display[i][d]='e';
                //}
        //}
	display = d;
	mutex = m;
}

Missile* makeMissile(){
	Missile* missile = malloc(sizeof(Missile));
	missile->row = 0;
	missile->col = rand()%maxX;
	return missile;
}

void destroyMissile(Missile* missile){
	if(missile!=NULL){
		free(missile);
	}
}

int collition(Missile* missile){
	pthread_mutex_lock(&mutex);
	char ch;
	if(display[missile->row+1][missile->col]){
		ch=display[missile->row+1][missile->col];
		if((ch=='_') || (ch =='|') || (ch== '#')){
			pthread_mutex_unlock(&mutex);
			return ch;
		}
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}

void* runMissile(void* missile){
	//generate a random number between 1 and maxX-1 sleep a random time and move down
	Missile* m = missile;
	pthread_mutex_lock(&mutex);
	mvprintw(m->row,m->col,"|");
	refresh();
	pthread_mutex_unlock(&mutex);
	while(1){
		if(collition(m)){
			break;
		}
		if(m->row==maxY){
			break;
		}
		pthread_mutex_lock(&mutex);
		mvprintw(m->row,m->col," ");
		m->row=m->row+1;
		mvprintw(m->row,m->col,"|");
		refresh();
		pthread_mutex_unlock(&mutex);
		usleep(300000);
		//usleep(10000);
	}
	if(collition(m)=='#'){
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
		//display[m->row+1][m->col]='e';
		//display[m->row+2][m->col]='_';
		refresh();
		mvprintw(m->row+1,m->col,"?");
		refresh();
		pthread_mutex_unlock(&mutex);
	}
	//do collision stuff;

	pthread_exit(0);

}



