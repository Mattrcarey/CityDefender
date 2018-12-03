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


void buildMap(City* city,int x, int y,char** display){
	int i = 0;
	while(i!=x){
		if(i==city->size){
			break;
		}
		mvprintw(y-(city->map[i]),i,"_");
		display[y-city->map[i]][i]='_';
		refresh();
		if(i>0&&city->map[i]>city->map[i-1]){
			for(int j=city->map[i-1];j<(city->map[i]);j++){
				mvprintw(y-j,i-1,"|");
				display[y-j][i-1]='|';
				refresh();
			}
		}
		if(i>0&&city->map[i]<city->map[i-1]){
			for(int j=city->map[i];j<(city->map[i-1]);j++){
				mvprintw(y-j,i,"|");
				display[y-j][i]='|';
				refresh();
			}
		}
		i++;
	}
	refresh();
}



City* readFile(FILE* fp){
	City* city = malloc(sizeof(City));
	char buf[256];
	city->missiles = -1;
	city->size = 0;
	int num;
	while(fgets(buf,sizeof(buf),fp)!=NULL){
		buf[strlen(buf)-1]='\0';
		if(buf[0]!='#'){
			if(city->defence==NULL){
				city->defence=malloc(strlen(buf));
				strcpy(city->defence,buf);
			}
			else if(city->attack==NULL){
				city->attack=malloc(strlen(buf));
				strcpy(city->attack,buf);
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
	char** display= malloc(height+1);
	for(int i = 0; i<height;i++){
		display[i] = malloc(width);
		for(int y=0; y<width;y++){
			display[i][y]='e';
		}	
	}
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	init_missiles(width,height,display,mutex);
	init_defence(width,height,display,mutex);
	buildMap(city,width,height,display);
	
	
	//starting the defence thread
	Defence* defence = make_defence(height-(tallesttower(city,width)+1),width/2);
	pthread_t defenceThread;
	pthread_create(&defenceThread,NULL,run,defence);
	

	//starting the missile threads test;
	pthread_t missilethread1;
	pthread_t missilethread2;
	pthread_t missilethread3;
	pthread_t missilethread4;
	Missile* missile1 = makeMissile();
	Missile* missile2 = makeMissile();
	Missile* missile3 = makeMissile();
	Missile* missile4 = makeMissile();
	for (int c =1; c<= 10000; c++){
		for (int d = 1; d<=10000; d++){
		}
	}
	int round=0;
	while(missilecount(round,city)){
		pthread_create(&missilethread1,NULL,runMissile,missile1);
		sleep(3);
		pthread_create(&missilethread2,NULL,runMissile,missile2);
		sleep(3);
		pthread_create(&missilethread3,NULL,runMissile,missile3);		
		sleep(3);
		pthread_join(missilethread1,NULL);
                pthread_join(missilethread2,NULL);
                pthread_join(missilethread3,NULL);
		destroyMissile(missile1);
		destroyMissile(missile2);
		destroyMissile(missile3);
		missile1 = makeMissile();
		missile2 = makeMissile();
		missile3 = makeMissile();
		round++;
	}
	for (int c = 1; c <= 100000; c++){
		for (int d = 1; d <= 100000; d++){
		}
	}
	endwin();
	return 1;

}
