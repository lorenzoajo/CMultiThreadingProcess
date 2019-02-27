/*
 * DA COMPLETARE PER CASA
 * 
 * Usare il numeri.py per generare lunghi elenchi di primi su cui testare il programma
 * 
 * Programma di esempio del paradigma 1 producer N consumer
 * i dati vengono messi su un buffer in cui il producer scrive e i consumer 
 * leggono. In principio il buffer va bene di qualsiasi dimensione: 
 * piu' e' grande maggiore e' il lavoro pronto da svolgere nel caso
 * il produttore rimanga bloccato (ad esempio a leggere dal disco)
 * 
 * */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>      
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#define Buf_size 10

// funzione per stabilire se n e' primo  
bool primo(int n)
{
	if(n<2) return false;
	if(n%2==0) return (n==2);
	for (int i=3; i*i<=n; i += 2)
	  if(n%i==0) return false;
	return true;
}

// struct contenente i parametri di input e output di ogni thread 
typedef struct {
	int quanti; // output
	long somma;
	int *buffer; 
	int *cindex;
	pthread_mutex_t *mutex_consumers;
	sem_t *sem_free_slots;
	sem_t *sem_ready;  
} dati;


// funzione eseguita dai thread consumer
void *tbody(void *a)
{  
	int n;
	dati *arg = (dati *)a; 
	arg->quanti = 0;
	arg->somma = 0;
	int *cindex = arg->cindex;
  // continua a leggere dal buffer fino a quando non incontra 0  
  while(1) {
    int e = sem_wait(arg->sem_ready);
    assert(e==0);
    // solo un consumer puo' avere accesso al buffer simultaneamente
    e = pthread_mutex_lock(arg->mutex_consumers);
    assert(e==0);
    n = buffer[(*cindex)++ % Buf_size];
    e = pthread_mutex_unlock(arg->mutex_consumers);
    assert(e==0);
    int e = sem_post(arg->sem_free_slots);
    assert(e==0);
    if(n==0) break; // letto il valore speciale, termino
    if(primo(n)) {  // valuta se n e' primo ed eventualmente aggiorna somma e quanti
      arg->quanti++;
      arg->somma += n;
    }
  }
  pthread_exit(NULL); 
}     


int main(int argc, char *argv[])
{
	// leggi input
	if(argc!=3) {
		printf("Uso\n\t%s file numthread\n", argv[0]);
		exit(1);
	}
	int p = atoi(argv[2]);
	assert(p>0);
	int tot_primi = 0;
	long tot_somma = 0;
	int i,e,n;  
	  
	// threads related
	int buffer[Buf_size];
	int cindex=0, pindex=0;
	pthread_t t[p];
	dati a[p];
	sem_t sem_free_slots, sem_ready;
	pthread_mutex_t mutex_consumers;

	// DA SCRIVERE inizializzazione semafori, creazione threads, altro?


	// read file and count primes 
	FILE *f = fopen(argv[1],"r");
	if(f==NULL) {perror("Errore apertura file"); return 1;}
	
	while(true) {
	e = fscanf(f,"%d", &n);
	if(e!=1) break; // se il valore e' letto correttamente e==1
	assert(n>0);    // i valori del file devono essere positivi
	// mette il valore n nel buffer per essere elaborato
	e = sem_wait(&sem_free_slots);
	assert(e!=0);
	buffer[pindex++ % Buf_size] = n;
	e = sem_post(&sem_ready);
	assert(e!=0);
	}
	
	// scrivere il valore 0 per far terminare i thread
	for(int i=0;i<p;i++) {
		e = sem_wait(&sem_free_slots);
		assert(e!=0);
		buffer[pindex++ % Buf_size] = 0;
		e = sem_post(&sem_ready);
		assert(e!=0);
	}

	// DA SCRIVERE: join dei thread e calcolo risulato
	fprintf(stderr,"Trovati %d primi con somma %ld\n",tot_primi,tot_somma);
	return 0;
}

