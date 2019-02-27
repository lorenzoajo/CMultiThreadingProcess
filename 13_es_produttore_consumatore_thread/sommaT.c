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
  long somma;   // output
  int **buffer; 
  int *cindex;
  pthread_mutex_t *mutex_consumers;
  sem_t *sem_free_slots;
  sem_t *sem_data_items; 
  int bsize; 
} dati;


// funzione eseguita dai thread consumer
void *tbody(void *arg)
{  
  dati *a = (dati *)arg;
  
  a->somma = 0;
  while(true) {
    int e = sem_wait(a->sem_data_items);
    assert(e==0);
    pthread_mutex_lock(a->mutex_consumers);
    int *vet = a->buffer[*(a->cindex)];
    *(a->cindex) = (*(a->cindex) + 1) % Buf_size;
    pthread_mutex_unlock(a->mutex_consumers);
    e = sem_post(a->sem_free_slots);
    assert(e==0);
    if(vet ==NULL) break;
    for(int i=0; i< a->bsize;i++){
    	a->somma += vet[i];
    }
    free(vet); //thread rilascia memoria allocata per lui dal produttore
  }
  
  pthread_exit(NULL); 
}     


int main(int argc, char *argv[])
{
  // leggi input
  if(argc!=4) {
    printf("Uso\n\t%s infile bsize numt\n", argv[0]);
    exit(1);
  }
  char *infile = argv[1];
  int bsize = atoi(argv[2]);
  if(bsize<0){
  	printf("bsize deve esse >0\n");
  	exit(1);
  }
  int numt = atoi(argv[3]);
  if(numt<0){
  	printf("numt deve esse >0\n");
  	exit(1);
  }
  int i,e;
  FILE *fin = fopen(infile,"r");
  if(fin == NULL){
  	printf("errore apertura file\n");
  	exit(1); 
  }
  // threads related
  int *buffer[Buf_size];
  int cindex=0, pindex=0;
  pthread_t t[numt];
  dati a[numt];
  sem_t sem_free_slots, sem_data_items;
  pthread_mutex_t mutex_consumers = PTHREAD_MUTEX_INITIALIZER;
  
  // inizializzazione semafori
  e = sem_init(&sem_free_slots, 0, Buf_size);
  assert(e==0);
  e = sem_init(&sem_data_items, 0, 0);
  assert(e==0);
  
  //  creo i thread consumatori
  for(i=0;i<numt;i++) {
    a[i].buffer = buffer;
    a[i].cindex = &cindex;
    a[i].mutex_consumers = &mutex_consumers;
    a[i].sem_data_items  = &sem_data_items;
    a[i].sem_free_slots  = &sem_free_slots;
    a[i].bsize = bsize;
    e = pthread_create(&t[i], NULL, tbody, (void *) &a[i]);
    assert(e==0);
  }

  int eR; //valore ritorno fread
  int *tmp;
  // read file 
  while(true) {
  	tmp = malloc(sizeof(int)*bsize);
    eR = fread(tmp,sizeof(int),bsize,fin);
    if(eR != bsize) break; //andrà gestito dal padre

      e = sem_wait(&sem_free_slots);
      assert(e==0);
      buffer[pindex] = tmp;	//passaggio del puntatore
      pindex = (pindex+1)%Buf_size;
      e = sem_post(&sem_data_items);
      assert(e==0);
    
  }
    //calcolo eventuale somma di numeri blocco < buffsize o controllo errore fread
  int somma = 0;
  if(eR > 0){
  	for(int i=0;i<eR;i++){
  		somma += tmp[i];
  	}
  	free(tmp);
  }
  if(eR < 0) perror("errore fread\n");
  
  
  
  // segnalo terminazione mettendo NULL
  for(int i=0;i<numt;i++) {
    e = sem_wait(&sem_free_slots);
    assert(e==0);
    buffer[pindex] = NULL;
    pindex = (pindex+1)%Buf_size;
    e = sem_post(&sem_data_items);
    assert(e==0);
  }


  // join dei thread e aggiunta a somma
  for(i=0;i<numt;i++) {
    e = pthread_join(t[i], NULL);
    if(e==0) {
      somma += a[i].somma;
    }
    else 
      fprintf(stderr,"Errore join %d\n",e);
  }

  fprintf(stderr,"somma %d\n",somma);
  return 0;
}



































