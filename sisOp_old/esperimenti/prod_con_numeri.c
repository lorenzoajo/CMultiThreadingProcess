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
//il produttore mette sul buffer numeri
//i consumatori leggono da buffer e fanno la somma
//poi la somma deve essere restiruita al produttore

void *produttore(void *a);
void *consumatore(void *a);


typedef struct{
	int buffer[N];	
	int next_in;
	int next_out;
}dati;

pthread_mutex_t mutex;
sem_t full,empty; 


int main(int argc, char**argv){
	
	
	int i;
	pthread_t thread_handler[4];  
	dati buff;
	buff.next_in = 0;
	buff.next_out = 0;
	
	
	//inizializzazione semafori
	sem_init(&full,0,10);
	sem_init(&empty,0,0);
	pthread_mutex_unlock(&mutex);
	
	

	//crea produttore
	if(pthread_create(&thread_handler[0],NULL,produttore,(void*) &buff)){
		perror("errore pthread_create\n");
		exit(3);
	}

	
	//crea 3 consumatori
	for(i=1; i < (TOT_THERAD); i++){
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
	
	
	return 0;
}




//il produttore legge mette nel buffer
void *produttore(void *a){
	dati *buff = (dati *) a;
	
	int i;
	for(i=100;i<0;i--){
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		
		buff->buffer[buff->next_in]= i ;
		buff->next_in = (buff->next_in+1) % N;
		
		sem_post(&empty);
		pthread_mutex_unlock(&mutex);
	}
	
	pthread_exit(NULL);
}





//i consumatori leggono da buffere e scrivono sul file di output
void *consumatore(void *a){
	dati *buff = (dati *) a;
	int sum = 0;
	
	while(1){
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		if(buff->buffer[buff->next_out]==0) break;
		sum += buff->buffer[buff->next_out];
		buff->next_out = (buff->next_out+1) % N;
		sem_post(&full);
		pthread_mutex_unlock(&mutex);
	}

	pthread_exit(NULL);
}

















