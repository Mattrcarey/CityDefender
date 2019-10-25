#define _DEFAULT_SOURCE
#include<string.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<curses.h>
#include<unistd.h>
#include<ctype.h>
#include"missile.h"
#include"defence.h"

//@author Matthew Carey

//this struct is used to store all the data from the 
//file
typedef struct City{
	char* defence;
	char* attack;
	int missiles;
	int map[300];
	int size;
}City;


///this function is used to return the height of the tallest tower 
///on the window
///@param city: the city struct contains the array of building heights
///@param width: is the width of the window
///@returns the max height
int tallesttower(City* city,int width){
	int max = 0;
	for(int i = 0; i<city->size;i++){
		if(i==width){
			return max;
		}
		if(city->map[i]>max){
			max = city->map[i];
		}
	}
	return max;
}


///this functions builds the city in the curses window
///@param city: city contains the array of building heights
///@param x: the width of the window
///@param y: the height of the window
void buildMap(City* city,int x, int y){
	int i = 0;
	mvprintw(0,(x/2)-24,"Enter 'q' to quit at end of attack, or control-C");
	refresh();
	while(i!=x){
		if(i==city->size){
			break;
		}
		mvprintw(y-(city->map[i]),i,"_");
		refresh();
		if(i>0&&city->map[i]>city->map[i-1]){
			mvprintw(y-(city->map[i]),i," ");
			for(int j=2;j<(city->map[i]);j++){
				mvprintw(y-j,i,"|");
				refresh();
			}
		}
		if(i>0&&city->map[i]<city->map[i-1]){
			for(int j=2;j<(city->map[i-1]);j++){
				mvprintw(y-j,i,"|");
				refresh();
			}
		}
		i++;
	}
	while(i!=x){
		mvprintw(y-2,i,"_");
		refresh();
		i++;
	}
}


///this function reads the file and builds a city struct
///@param fp: this is the file we read from
///@returns a city struct if no errors are encountered
City* readFile(FILE* fp){
	City* city = malloc(sizeof(City));
	char buf[256];
	city->missiles = -1;
	city->defence = NULL;
	city->attack = NULL;
	city->size = 0;
	int num;
	while(fgets(buf,sizeof(buf),fp)!=NULL){
		buf[strlen(buf)-1]='\0';
		if(buf[0]!='#'){
			if(city->defence==NULL){
				city->defence = strdup(buf);
			}
			else if(city->attack==NULL){
				city->attack = strdup(buf);
			}
			else if(city->missiles == -1){
				if(atoi(buf)<0){
					fprintf(stderr,"Error: negative missiles\n");
					exit(0);
				}
				city->missiles = atoi(buf);
			}
			else if(buf[0]>47&&buf[0]<58){
				char* token = strtok(buf," ");
				while(token!=NULL){
					num = atoi(token);
					city->map[city->size]=num;
					city->size = city->size+1;
					token = strtok(NULL," ");
				}
			}
		}
	}
	if(city->defence==NULL){
		fprintf(stderr,"Error: missing defender name.\n");
		free(city);
		exit(0);
	}
	if(city->attack==NULL){
		fprintf(stderr,"Error: missing attacker name.\n");
		free(city->defence);
		free(city);
		exit(0);
	}
	if(city->missiles == -1){
		fprintf(stderr,"Error: missing missile number.\n");
		exit(0);
	}
	if(city->size == 0){
		fprintf(stderr,"Error: missing grid.\n");
		exit(0);
	}
	return city;
}


///calls function to read the file, initialised the curses window and starts 
//the threads that run the game
int main(int argc, char* argv[]){
	FILE *fp;
	City* city;
	if(argc<2){
		fprintf(stderr,"Usage: ./threads config-file");
		exit(0);
	}
	if((fp=fopen(argv[1],"r"))){
		city = readFile(fp);
		fclose(fp);
	}
	else{
		fprintf(stderr,"Invalid file");
		exit(0);
	}
	initscr();
	cbreak();
	noecho();
	int width = getmaxx(stdscr);
	int height = getmaxy(stdscr);
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	init_missiles(width,height,mutex);
	init_defence(width,height,mutex);
	buildMap(city,width,height);
	
	//starting the defence thread
	Defence* defence = make_defence(height-(tallesttower(city,width)+2),width/2);
	pthread_t defenceThread;
	pthread_create(&defenceThread,NULL,run,defence);

	//starting the missile threads
	pthread_t missilethread;
	if(city->missiles==0){//if we have infinite missiles
		while(1){
			pthread_create(&missilethread,NULL,runMissile,makeMissile());
			pthread_detach(missilethread);
			sleep((rand()%4)+1);
		}
	}
	else{
		pthread_t threadlist[city->missiles];
		for(int i = 0; i<city->missiles; i++){
			pthread_create(&threadlist[i],NULL,runMissile,makeMissile());
		       	sleep((rand()%4)+1);		
		}
		for(int i = 0; i<city->missiles; i++){
			pthread_join(threadlist[i],NULL);
		}
	}

	//the attack is over at this point
	pthread_mutex_lock(&mutex);
	mvprintw(3,(width/2)-24,"The ");
	mvprintw(3,(width/2)-20,city->attack);
	mvprintw(3,(width/2)-(20)+strlen(city->attack)," attack has ended");
	refresh();
	pthread_mutex_unlock(&mutex);
	gameOff();
	pthread_join(defenceThread,NULL);//once the defence thread joins
	pthread_mutex_lock(&mutex);	 //the the defence had surrendered
	mvprintw(5,(width/2)-24,"The ");
        mvprintw(5,(width/2)-20,city->defence);
        mvprintw(5,(width/2)-(20)+strlen(city->defence)," defence has ended");
	mvprintw(6,(width/2)-24,"hit enter to close...");
	refresh();
	pthread_mutex_unlock(&mutex);
	int c = getchar();
	while(1){
		if(c == 13){//game ends on enter
			break;
		}
		c=getchar();
	}
	destroy_defence(defence);
	free(city->defence);
	free(city->attack);
	free(city);
	endwin();
	return 1;
}
