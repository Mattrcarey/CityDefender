
#ifndef _MISSILE_H
#define _MISSILE_H

typedef struct Missile{
	int col;
	int row;
}Missile;

void init_missiles(int x, int y,char** display,pthread_mutex_t m);

Missile* makeMissile();

void destroyMissile(Missile* missile);

void* runMissile(void* missile);


//void buildMap(int *layout, int size);


#endif