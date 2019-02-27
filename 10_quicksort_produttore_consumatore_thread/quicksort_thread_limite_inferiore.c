#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>      
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#define Buf_size 10

// struct contenente i parametri del buffer
typedef struct {
	int p;
	int r; 
} quick_struct;

//struct argomento thread
typedef struct {
  quick_struct *buffer; 
  int *a;
  int *cindex;
  pthread_mutex_t *mutex_consumers;
  sem_t *sem_free_slots;
  sem_t *sem_data_items;  
} dati;

int partition(int A[], int p, int r)
{
  int t,x;
  int i,j;

  // il pivot sara' il mediano tra A[p],A[i],A[r]
  i = (p+r)/2;
  if( A[p] > A[i] )
      {t=A[i];A[i]=A[p];A[p]=t;} // scambia A[i]<->A[p] 
  if( A[p] > A[r] )
      {t=A[r];A[r]=A[p];A[p]=t;} // scambia A[r]<->A[p], ora A[p] e' il minino
  if( A[i] > A[r] )
      {t=A[r];A[r]=A[i];A[i]=t;} // scambia A[r]<->A[i], ora A[p]<= A[i] <= A[r]
  
  x = A[i];                   // pivot
  t=A[i];A[i]=A[p];A[p]=t;    // scambia A[i]<->A[p]      

  i= p-1; j= r+1;
  while(1) {
    while(A[--j]>x) ; // scan da destra a sinistra: esce se A[j]<=x
    while(A[++i]<x) ; // scan da sinistra a destra: esce se A[i]>=x
    if(i<j) {         
      t=A[i]; A[i]=A[j]; A[j]=t;  // scambia A[i] <-> A[j] e riprendi scansioni
    }
    else break;       // se i>=j termina
  } 
  return j;
}
void quicksort(int *A, int p, int r)       {
  int q;
  
  if(p<r) {  // se p==r  c'e'un solo elemento e quindi non si deve ordinare
    q = partition(A,p,r);
    quicksort(A,p,q);
    quicksort(A,q+1,r);
  }
}

int *random_array(int n, int seed)
{
  assert(n>0);
  srand(seed);
  int *a = malloc(n* sizeof(int));
  assert(a!=NULL);
  for(int i=0;i<n;i++)
    a[i] = (int) rand() % 1000000;
  return a;
}

// crea una copia dell'array a 
int *copy_array(int *a, int n)
{
  int *b = (int *) malloc(n* sizeof(int));
  assert(b!=NULL);
  for(int i=0;i<n;i++)
    b[i] = a[i];
  return b;
}

// verifica che l'array a[0..n] sia uguale a b[0..n]
bool check_sort(int *a, int *b, int n)
{
  for(int i=0;i<n;i++)
     if(a[i]!=b[i]) return false;
  return true;
}


// funzione di confronto tra interi passata a qsort
int intcmp(const void *a, const void *b)
{
  return *((int *) a) - *((int *) b);
}

// funzione eseguita dai thread consumer
void *tbody(void *arg){
  dati *a = (dati *)arg;
  
  while(true) {
    int e = sem_wait(a->sem_data_items);
    assert(e==0);
    pthread_mutex_lock(a->mutex_consumers);
    int p = a->buffer[*(a->cindex)].p;
    int r = a->buffer[*(a->cindex)].r;
    *(a->cindex) = (*(a->cindex) + 1) % Buf_size;
    pthread_mutex_unlock(a->mutex_consumers);
    e = sem_post(a->sem_free_slots);
    assert(e==0);
    if(p == -1 || r == -1) break;
    quicksort(a->a,p,r);
  }
  pthread_exit(NULL); 
}  

void very_quicksort(int *a, int p, int r, int lim, sem_t * sem_data_items, sem_t *sem_free_slots, int *pindex, quick_struct *buffer){
	int e;
	if(p < r) {
		if (r-p <= lim){
		  	//scrivi coppia (p,q) sul buffer
		  	e = sem_wait(sem_free_slots);
			assert(e==0);
			buffer[*pindex].p = p;
			buffer[*pindex].r = r;
			*pindex = ((*pindex)+1)%Buf_size;
			e = sem_post(sem_data_items);
			assert(e==0);
		}
		else {
		  // esegui come nel caso senza thread:
		  int q =  partition(a,p,r);
		  //ordina ricorsivamente A[p] ... A[q]
		  very_quicksort(a,p,q, lim, sem_data_items,sem_free_slots,pindex,buffer);
		  //ordina ricorsivamente A[q+1] .. A[r]
		  very_quicksort(a,q+1,r, lim, sem_data_items,sem_free_slots,pindex,buffer);
		}
	}
}

int main(int argc, char *argv[])
{
	int seed;
  // leggi input
  if(argc<4 || argc>5) {
    printf("Uso\n\t%s n p lim [seed]\n", argv[0]);
    exit(1);
  }
  int n = atoi(argv[1]);	//numero elementi vettore
  assert(n>=0);
  int p = atoi(argv[2]);	//numero di thread
  assert(p>=0);
  int lim = atoi(argv[3]);	//limite per chiamare sotto-thread
  assert(lim>=0);
   //------se seed opzionale è settato viene letto, altrimenti default 3141562
    if(argc==5)
    seed = atol(argv[4]);
  else
    seed = 3141562;
   
  // genera array random
  int *a = random_array(n,seed);
  // copia array e ordina la copia usando la funzione di libreria
  int *copia = copy_array(a,n);
  qsort(copia,n,sizeof(int), intcmp);	//funzione di libreria ordina la copia
  
  int i,e;    
  // threads related
  quick_struct buffer[Buf_size];
  int cindex=0, pindex=0;
  pthread_t t[p];
  dati arg[p];
  sem_t sem_free_slots, sem_data_items;
  pthread_mutex_t mutex_consumers = PTHREAD_MUTEX_INITIALIZER;
  
  // inizializzazione semafori
  e = sem_init(&sem_free_slots, 0, Buf_size);
  assert(e==0);
  e = sem_init(&sem_data_items, 0, 0);
  assert(e==0);
  //  creo i thread consumatori
  for(i=0;i<p;i++) {
    arg[i].buffer = buffer;
    arg[i].cindex = &cindex;
    arg[i].mutex_consumers = &mutex_consumers;
    arg[i].sem_data_items  = &sem_data_items;
    arg[i].sem_free_slots  = &sem_free_slots;
    arg[i].a = a;
    e = pthread_create(&t[i], NULL, tbody, (void *) &arg[i]);
    assert(e==0);
  }
  
  //quando questa funzione void torna il vettore dovrebbe essere ordinato
  very_quicksort(a,0,n-1, lim, &sem_data_items,&sem_free_slots,&pindex,buffer);
   
   // segnalo terminazione mettendo p e r a -1
  for(int i=0;i<p;i++) {
    e = sem_wait(&sem_free_slots);
    assert(e==0);
    buffer[pindex].p = -1;
    buffer[pindex].r = -1;
    pindex = (pindex+1)%Buf_size;
    e = sem_post(&sem_data_items);
    assert(e==0);
  }
  
   // join dei thread e calcolo risultato
  for(i=0;i<p;i++) {
    e = pthread_join(t[i], NULL);
    if(e==0) {
      fprintf(stderr,"Thread %d terminato\n",i);
    }
    else 
      fprintf(stderr,"Errore join %d\n",e);
  }
  
  //controllo che sia corretto l'ordinamento
  if(check_sort(a,copia,n)){
  	printf("tutto ok\n");
  }
  else printf("errore\n");
 
  //stampa del vettore ordinato con thread
  for(int i =0; i<n;i++){
  	printf("a{%d}:\t%d   %d\n",i,a[i],copia[i]);
  }

	return 0;
}









