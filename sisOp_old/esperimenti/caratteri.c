#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h> 
#include <signal.h>     
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TOT_THERAD 4
#define N 10

//proviamo a fare un buffer produtore e 3 consumatori
//il produttore legge da file e mette su buffer 
//i consumatori leggono da buffer e mettono su file output

void *produttore(void *a);
void *consumatore(void *a);


typedef struct{
	char buffer[N];
	int next_in;
	int next_out;
	FILE* fout;
	FILE* fin;
}dati;

pthread_mutex_t mutex;
sem_t full,empty; 


int main(int argc, char**argv){
	
	if(argc<2){
		perror("file input mancante, richiama da riga di comando\n");
		exit(1);
	}
	
	int i;
	pthread_t thread_handler[4];  
	dati buff;
	buff.next_in = 0;
	buff.next_out = 0;
	
	
	//inizializzazione semafori
	sem_init(&full,0,10);
	sem_init(&empty,0,0);
	pthread_mutex_unlock(&mutex);
	
	//apertura file input e output
	FILE *fin, *fout;
	fin=NULL;fout=NULL;
	fin = fopen(argv[1],"r");
	fout = fopen("output.txt","w");
	if(fin==NULL || fout==NULL){
		perror("errore fopen\n");
		exit(2);
	}
	
	buff.fin = fin;
	buff.fout = fout;
	
	

	//crea produttore
	if(pthread_create(&thread_handler[0],NULL,produttore,(void*) &buff)){
		perror("errore pthread_create\n");
		exit(3);
	}

	
	//crea 3 consumatori
	for(i=1; i < (TOT_THERAD-1); i++){
		if(pthread_create(&thread_handler[i],NULL,consumatore,(void*) &buff)){
			perror("errore pthread_create\n");
			exit(4);
		}
	}
	

	//aspetta join dei thread
	for(i=0; i < TOT_THERAD;i++){
		pthread_join(thread_handler[i],NULL);
	}

	
	//distruzione semafori vari
	sem_destroy(&full);
	sem_destroy(&empty);
	pthread_mutex_destroy(&mutex);
	
	
	fflush(fout);
	
	//chiusura dei file
	fclose(fin);
	fclose(fout);
	return 0;
}




//il produttore legge mette nel buffer
void *produttore(void *a){
	dati *buff = (dati *) a;
	
	while(1){
		sem_wait(&full);
		pthread_mutex_lock(&mutex);

		buff->buffer[buff->next_in] = fgetc(buff->fin);
		if(buff->buffer[buff->next_in]==EOF) break;
		buff->next_in= (buff->next_in + 1) % N;
		
		sem_post(&empty);
		pthread_mutex_unlock(&mutex);
	}
	
	
	pthread_exit(NULL);
}





//i consumatori leggono da buffere e scrivono sul file di output
void *consumatore(void *a){
	dati *buff = (dati *) a;
	char check;
	

	while(1){
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);

		check = fputc(buff->buffer[buff->next_out],buff->fout);
		buff->next_out = (buff->next_out + 1) % N;
		if(check == EOF) break;
		
		sem_post(&full);
		pthread_mutex_unlock(&mutex);
	}

	pthread_exit(NULL);
}






































