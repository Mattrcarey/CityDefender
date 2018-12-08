
#ifndef _DEFENCE_H
#define _DEFENCE_H

//@author Matthew Carey

//the structure to represent a defence thread
typedef struct Defence{
        int row;
        int col;
        char* graphic;
}Defence;


//I included all the comments for these functions
//in the defence.c file

void init_defence(int x,int y,pthread_mutex_t m);

Defence* make_defence(int r, int c);

void destroy_defence(Defence* defence);

void gameOff();

void* run(void * defence);

#endif
