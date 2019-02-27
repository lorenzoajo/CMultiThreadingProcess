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

void *tbody(void *arg);
//struct argomento thread
typedef struct {
  int *a;
  int p;
  int r;
  int lim;
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
void quicksort(int *A, int p, int r, int lim){
  int q,e;
  pthread_t t;
  bool aiutato = false;
  
  if(p<r) {  // se p==r  c'e'un solo elemento e quindi non si deve ordinare
  	aiutato = false;
  	q = partition(A,p,r);
  	if(r-q > lim){
  		//crea thread che ordina seconda parte A[q+1] .. A[r] 
  		dati arg;
  		arg.a = A;
  		arg.p = q+1;
  		arg.r = r;
  		arg.lim = lim;
  		e = pthread_create(&t, NULL, tbody, (void *) &arg);
  		assert(e==0);
  		aiutato = true;
  		quicksort(A,p,q,lim);
  	}
	else{
		quicksort(A,q+1,r,lim);
		quicksort(A,p,q,lim);
	}
  }
  if(aiutato == true){
  		e = pthread_join(t, NULL);
  		if(e!=0) {
      		fprintf(stderr,"Errore join %d\n",e);
      	}
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
  
  quicksort(a->a,a->p,a->r,a->lim);
  
  pthread_exit(NULL); 
}  


int main(int argc, char *argv[])
{
	int seed;
  // leggi input
  if(argc<3 || argc>4) {
    printf("Uso\n\t%s n lim [seed]\n", argv[0]);
    exit(1);
  }
  int n = atoi(argv[1]);	//numero elementi vettore
  assert(n>=0);
  int lim = atoi(argv[2]);	//limite per chiamare altro-thread
  assert(lim>=0);
   //------se seed opzionale è settato viene letto, altrimenti default 3141562
    if(argc==4)
    seed = atol(argv[3]);
  else
    seed = 3141562;
   
  // genera array random
  int *a = random_array(n,seed);
  // copia array e ordina la copia usando la funzione di libreria
  int *copia = copy_array(a,n);
  qsort(copia,n,sizeof(int), intcmp);	//funzione di libreria ordina la copia
 
  
  //quando questa funzione void torna il vettore dovrebbe essere ordinato
  quicksort(a,0,n-1,lim);
   
  
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









