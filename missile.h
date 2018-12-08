
#ifndef _MISSILE_H
#define _MISSILE_H

//@author Matthew Carey

//the structure to represent a missile
typedef struct Missile{
	int col;
	int row;
	int speed;
}Missile;

//I included all the comments for these function 
//in the missile.c file

void init_missiles(int x, int y,pthread_mutex_t m);

Missile* makeMissile();

void destroyMissile(Missile* missile);

void* runMissile(void* missile);


#endif
