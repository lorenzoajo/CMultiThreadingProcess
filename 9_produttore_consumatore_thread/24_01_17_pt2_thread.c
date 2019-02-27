#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>      
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#define Buf_size 10


// struct contenente i parametri di input e output di ogni thread 
typedef struct {
  int *buffer; 
  int *pindex;
  pthread_mutex_t *mutex_producers;
  sem_t *sem_free_slots;
  sem_t *sem_data_items; 
  FILE *fin; 
} dati;

// funzione eseguita dai thread producer
void *tbody(void *arg)
{  
  dati *a = (dati *)arg;
  int num;
  
  
  while(true) {
  	int e = sem_wait(a->sem_free_slots);
  	assert(e==0);
  	pthread_mutex_lock(a->mutex_producers);
  
  	if(fscanf(a->fin,"%d",&num) != 1){
  			num = -1;  //andrebbe usato una cond_variable però essendo quandrati sono sempre positivi, quindi mando un negativo e va bene come segnale
		  	//int e = sem_wait(a->sem_free_slots);
			//assert(e==0);
			//pthread_mutex_lock(a->mutex_producers);
			a->buffer[*(a->pindex)] = num;
			*(a->pindex) = (*(a->pindex) + 1) % Buf_size;
			pthread_mutex_unlock(a->mutex_producers);
			e = sem_post(a->sem_data_items);
			assert(e==0);
  		break;
  	}
  	else{
  		num = num *num; //quadrato
		//int e = sem_wait(a->sem_free_slots);
		//assert(e==0);
		//pthread_mutex_lock(a->mutex_producers);
		a->buffer[*(a->pindex)] = num;
		*(a->pindex) = (*(a->pindex) + 1) % Buf_size;
		pthread_mutex_unlock(a->mutex_producers);
		e = sem_post(a->sem_data_items);
		assert(e==0);
  	}
  }
  pthread_exit(NULL); 
}     

int main(int argc, char *argv[])
{
  // leggi input
  if(argc!=3) {
    printf("Uso\n\t%s n nomefile\n", argv[0]);
    exit(1);
  }
  int n = atoi(argv[1]);
  assert(n>=0);
  FILE *fin = fopen(argv[2],"r");
  if(fin==NULL) {perror("Errore apertura file"); return 1;}
  int i,e,num;    
  // threads related
  int buffer[Buf_size];
  int cindex=0, pindex=0;
  pthread_t t[n];
  dati a[n];
  sem_t sem_free_slots, sem_data_items;
  pthread_mutex_t mutex_producers = PTHREAD_MUTEX_INITIALIZER;
  
  // inizializzazione semafori
  e = sem_init(&sem_free_slots, 0, Buf_size);
  assert(e==0);
  e = sem_init(&sem_data_items, 0, 0);
  assert(e==0);
  //  creo i thread produttori
  for(i=0;i<n;i++) {
    a[i].buffer = buffer;
    a[i].pindex = &pindex;
    a[i].mutex_producers = &mutex_producers;
    a[i].sem_data_items  = &sem_data_items;
    a[i].sem_free_slots  = &sem_free_slots;
    a[i].fin = fin;
    e = pthread_create(&t[i], NULL, tbody, (void *) &a[i]);
    assert(e==0);
  }
	long int somma = 0;
	while(true){
		e = sem_wait(&sem_data_items);
		assert(e==0);
		num = buffer[cindex];
		cindex = (cindex+1)%Buf_size;
		e = sem_post(&sem_free_slots);
		assert(e==0);
		if(num ==-1) break;
		somma += num;
    }


  // join dei thread e chiusura file aperto con fin
  for(i=0;i<n;i++) {
    e = pthread_join(t[i], NULL);
    if(e==0) {
      fprintf(stderr,"Thread %d terminato\n",i);
    }
    else 
      fprintf(stderr,"Errore join %d\n",e);
  }
  fclose(fin);

  fprintf(stderr,"somma %ld\n",somma);
  return 0;
}

