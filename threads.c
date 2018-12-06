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

typedef struct City{
	char* defence;
	char* attack;
	int missiles;
	int map[1000];
	int size;
}City;


int missilecount(int current,City* city){
	if(city->missiles==0){
		return 1;
	}
	else if(current<city->missiles){
		return 1;
	}
	else{
		return 0;
	}
}


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


void buildMap(City* city,int x, int y){
	int i = 0;
	mvprintw(0,(x/2)-24,"Enter 'q' to quit at end of attack, or control-C");
	refresh();
	while(i!=x){
		if(i==city->size){
			break;
		}
		mvprintw(y-(city->map[i]),i,"_");
		//display[y-city->map[i]][i]='_';
		refresh();
		if(i>0&&city->map[i]>city->map[i-1]){
			mvprintw(y-(city->map[i]),i," ");
			//display[y-(city->map[i])][i]='e';
			for(int j=2;j<(city->map[i]);j++){
				mvprintw(y-j,i,"|");
				//display[y-j][i]='|';
				refresh();
			}
		}
		if(i>0&&city->map[i]<city->map[i-1]){
			for(int j=2;j<(city->map[i-1]);j++){
				mvprintw(y-j,i,"|");
				//display[y-j][i]='|';
				refresh();
			}
		}
		i++;
	}
	while(i!=x){
		mvprintw(y-2,i,"_");
		//display[y-2][i] = '_';
		refresh();
		i++;
	}
	//refresh();
}



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
				//city->defence=malloc(strlen(buf));
				//strcpy(city->defence,buf);
				city->defence = strdup(buf);
			}
			else if(city->attack==NULL){
				//city->attack=malloc(strlen(buf));
				//strcpy(city->attack,buf);
				city->attack = strdup(buf);
			}
			else if(city->missiles == -1){
				city->missiles = atoi(buf);
			}
			else if(buf[0]>47&&buf[0]<58){
				char* token = strtok(buf," ");
				while(token!=NULL){
					if(atoi(token)){
						num = atoi(token);
						city->map[city->size]=num;
						city->size = city->size+1;
					}
					token = strtok(NULL," ");
				}
			}
		}
	}
	return city;
}


int main(int argc, char* argv[]){
	//check that it has the right number of args
	//check that the arg works are a file
	//run helper fuction to read the file
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
	//char** display= malloc(height-1);
	//for(int i = 0; i<height;i++){
	//	display[i] = malloc(width-1);
	//	for(int y=0; y<width;y++){
	//		display[i][y]='e';
	//	}	
	//}
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	init_missiles(width,height,mutex);
	init_defence(width,height,mutex);
	buildMap(city,width,height);
	
	
	//starting the defence thread
	//int test = tallesttower;
	Defence* defence = make_defence(height-(tallesttower(city,width)+1),width/2);
	pthread_t defenceThread;
	pthread_create(&defenceThread,NULL,run,defence);
	//pthread_detach(defenceThread);

	//starting the missile threads test;
	pthread_t missilethread;
	int round=0;
	while(missilecount(round,city)){
		pthread_create(&missilethread,NULL,runMissile,makeMissile());
		pthread_detach(missilethread);
		sleep(3);
		//pthread_join(missilethread,NULL);
		round++;
	}
	sleep(3);
	pthread_mutex_lock(&mutex);
	mvprintw(3,(width/2)-24,"The ");
	mvprintw(3,(width/2)-20,city->attack);
	mvprintw(3,(width/2)-(20)+strlen(city->attack)," attack has ended");
	pthread_mutex_unlock(&mutex);
	int c= getchar();
	gameOff();
	pthread_join(defenceThread,NULL);
	pthread_mutex_lock(&mutex);
	mvprintw(5,(width/2)-24,"The ");
        mvprintw(5,(width/2)-20,city->defence);
        mvprintw(5,(width/2)-(20)+strlen(city->defence)," defence has ended");
	mvprintw(6,(width/2)-24,"hit enter to close...");
	c=getch();
	pthread_mutex_unlock(&mutex);
	while(1){
		if(c == 13){
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
