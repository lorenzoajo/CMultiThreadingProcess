#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>



// ===== Esempi di uso delle condition variables =====

// ========  Es heap con struct

// Uso di una Condition Variable per gestione di un heap di memoria 

// numero iniziale di bytes disponibili
#define MAX_HEAP_SIZE 100000

//struttura per gestire heap
typedef struct {
  int bytesLeft;             // bytes attualmente disponibili
  pthread_cond_t cond;    // condition variable
  pthread_mutex_t mutex;  // mutex associato alla condition variable
} heap;

// prototipi funzioni per allocazione e deallocazione memoria
// il codice non fornito
//non le uso perchè alloco e dealloco per finta !!!!!!!
/*
void *get_heapmemory(int);    
void free_heapmemory(void *);
*/

int heap_init(heap *h, int maxSize);
int allocate_memory(heap *h, int size);
int free_mem(heap *h, int size);


//struct heap variabile globale per farla vedere agli handler e ai thread
heap h;

typedef struct{
	int mem;
	int sec;
	//heap *h_ptr;	//lo vede già perche dichiarato globale
}struct_arg;

void *tbody(void *arg)
{
	int e;
	//tiene per sec secondi una quantità mem di memoria dal nostro heap
	struct_arg *d = (struct_arg *) arg;   
	printf("THREAD>> alloco %d memoria e la trattengo per %d secondi\n", d->mem, d->sec);
	e = allocate_memory(&h, d->mem);
	if(e == -1){
		printf("errore allocazione memoriadi taglia %d\tthread uscito\n",d->mem);
		pthread_exit(NULL);  
	}
	sleep(d->sec);
	free_mem(&h, d->mem);
	printf("THREAD>> liberato %d memoria trattenuta per %d secondi\n", d->mem, d->sec);

	pthread_exit(NULL);    
}

void handler1(int s)
{	
	int e;
	struct_arg arg;	//andrebbe fatta puntatore allocato con malloc perchè questo handler potrebbe terminare e il thread potrebbe ancora leggere da qui causando segmentation fault !!!
	//sarebbe il thread che, così, dopo aver letto faccia la free di tale struttura della quale abbiamio passato l'indirizzo dopo averla passaa con malloc
	pthread_t t;
	if(s==SIGUSR1) {
		printf("Segnale %d ricevuto dal processo, inserire mem(grandezza memoria da allocare e sec (per quanti secondi tenerla))\n", s);
		scanf("%d %d", &arg.mem, &arg.sec);
		e = pthread_create(&t, NULL, tbody, (void *) &arg);
		if(e!=0) {
			fprintf(stderr,"Errore pthread_create (%d)\n",e);
			exit(1);
		}
		e = pthread_join(t, NULL);  
		if(e!=0) {
			fprintf(stderr,"Errore pthread_join (%d)\n",e); 
			exit(1);
		} 
	}
}

void handler2(int s)
{
	if(s==SIGUSR2) {
		printf("quantita di memoria attualmente disponibile %d\n",h.bytesLeft);

	} 
}


int main(int argc, char **argv){
	
	//heap h; //messo come globale per farla vedere ai thread creati dai signal handler

	//inizializzazione variabili di gestione heap
	heap_init(&h, MAX_HEAP_SIZE);
	
	// definisce signal handler 
	struct sigaction sa1;
	sa1.sa_handler = handler1;
	sigemptyset(&sa1.sa_mask);     // setta a "insieme vuoto" sa.sa_mask maschera di segnali da bloccare 
	sa1.sa_flags = SA_RESTART;     // restart system calls  if interrupted
	sigaction(SIGUSR1,&sa1,NULL);  // handler per USR1
	
	// definisce signal handler 
	struct sigaction sa2;
	sa2.sa_handler = handler2;
	sigemptyset(&sa2.sa_mask);     // setta a "insieme vuoto" sa.sa_mask maschera di segnali da bloccare 
	sa2.sa_flags = SA_RESTART;     // restart system calls  if interrupted
	sigaction(SIGUSR2,&sa2,NULL);  // handler per USR1
	
	fprintf(stderr,"per mandare segnali il mio pid è: %d",getpid());
	
	while(true){
		//attende segnali (all'infinito, terminare con CTRL-c)
		pause();
	}
	
	return 0;
}


// la cv c e' protetta dal mutex m e si riferisce alla variabile bytesLeft
//   chiunque modifica byteLeft deve farlo sotto il lock di m
//   per mettersi in attesa del segnale c o per mandare il segnale c
//   si deve essere sotto il lock m 

// funzione chiamata da ogni thread che ha bisogno di memoria
// se non c'e' abbastanza memoria attende che se ne liberi
//non serve
/*
void * allocate(int size) {
  pthread_mutex_lock(&m);
  while (bytesLeft < size)
    // aspetto che qualcuno attraverso c indichi che ci sono novita' su byteLeft
    pthread_cond_wait(&c, &m);  // il mutex viene rilascato dalla cond_wait
  // all'uscita della cond_wait il mutex e' ottenuto   
  void *ptr = get_heapmemory(size);  // ottiene il puntatore alla zona di memoria
  bytesLeft -= size;                 // aggiorna byteLeft
  pthread_mutex_unlock(&m);
  return ptr;
}
*/

//non serve
/*
// funzione chiamata da ogni thread che restituisce della memoria
void free_mem(void *ptr, int size) {
  pthread_mutex_lock(&m);
  free_heapmemory(ptr);        // restituisce la memoria usata
  bytesLeft += size;            // modifico byteLeft sotto mutex
  // tutti quelli che stanno aspettando su c sono avvertiti ...
  pthread_cond_broadcast(&c);   // ... ma non partono subito perche' c'e' il mutex
  pthread_mutex_unlock(&m);     // rilascio mutex
}
*/


int heap_init(heap *h, int maxSize){

	//inizializzazione variabili di gestione heap
	h->bytesLeft = maxSize;
	//h->cond = PTHREAD_COND_INITIALIZER;
	 pthread_cond_init(&h->cond,NULL);
	//h->mutex = PTHREAD_MUTEX_INITIALIZER;	
	pthread_mutex_init(&h->mutex, NULL);
	
	return 0;  //per dire tutto ok
}


int allocate_memory(heap *h, int size){
	int e = 0;
  	e = pthread_mutex_lock(&h->mutex);
  	if(e!=0) return e;
  	if(size > MAX_HEAP_SIZE) return -1; //errore richiede di allocare memoria più grossa del MAX_HEAP_SIZE
	while (h->bytesLeft < size){
    	// aspetto che qualcuno attraverso c indichi che ci sono novita' su byteLeft
    	e = pthread_cond_wait(&h->cond, &h->mutex);  // il mutex viene rilascato dalla cond_wait
    	if(e!=0) return e;
    }
  // all'uscita della cond_wait il mutex e' ottenuto   
 // void *ptr = get_heapmemory(size);  // ottiene il puntatore alla zona di memoria
 	h->bytesLeft -= size;                 // aggiorna byteLeft
	e = pthread_mutex_unlock(&h->mutex);
	 	if(e!=0) return e;
	return 0;
}



int free_mem(heap *h, int size){
	int e =0;
	
	e = pthread_mutex_lock(&h->mutex);
	if(e !=0) return e;
	//free_heapmemory(ptr);        // restituisce la memoria usata
	h->bytesLeft += size;            // modifico byteLeft sotto mutex
	// tutti quelli che stanno aspettando su c sono avvertiti ...
	e = pthread_cond_broadcast(&h->cond);   // ... ma non partono subito perche' c'e' il mutex
	if(e != 0) return e;
	e = pthread_mutex_unlock(&h->mutex);     // rilascio mutex
	if(e != 0) return e;

	return 0;
}


















