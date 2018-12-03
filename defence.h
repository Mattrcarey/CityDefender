
#ifndef _DEFENCE_H
#define _DEFENCE_H

typedef struct Defence{
        int row;
        int col;
        char* graphic;
}Defence;


void init_defence(int x,int y, char** display,pthread_mutex_t m);

Defence* make_defence(int r, int c);

void destroy_defence(Defence* defence);

void* run(void * defence);

#endif