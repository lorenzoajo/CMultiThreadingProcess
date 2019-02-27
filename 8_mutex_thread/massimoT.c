#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>      
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

typedef struct {
  int inizio;   // input
  int fine;
  int *arr;     // puntatore alla tabella dei primi 
  int max;
} dati;

// funzione eseguita dal thread
void *tbody(void *a)
{  
  dati *arg = (dati *)a; 
  fprintf(stderr,"Thread cerca da %d a %d\n",arg->inizio,arg->fine);
  arg->max = arg->arr[arg->inizio];
  for (int j=arg->inizio;j<arg->fine;j++)
    if(arg->max  < arg->arr[j]) arg->max  = arg->arr[j];
    
  pthread_exit(NULL); 
}

// prototipi di funzione
int *random_array(int n);
int massimo(int *a, int n, int p);

// main
int main(int argc, char **argv)
{
  if(argc!= 3)
  {
    fprintf(stderr,"Uso:\n\t %s n p\n",argv[0]);
    exit(1);
  }
  int n= atoi(argv[1]);
  assert(n > 0);
  int p = atoi(argv[2]);
  assert(p > 0);

  int *a = random_array( n );
  int max = a[0];
  for(int i=1; i < n ;i++)
    if(a[i]>max) max=a[i];
      
  printf("Massimo calcolato con un thread: %d\n",max); 
  
  // questa e' la funzione da scrivere per esercizio
  max = massimo(a,n,p);
  printf("Massimo calcolato con %d thread: %d\n",p, max); 

  free(a);
  return 0;
}

// calcola il massimo dell'array a[] di n elementi
// utilizzando p thread
// ogni thread deve leggere al piu' (n/p)+1 elementi 
int massimo(int *a, int n, int p) {
    dati argomenti[p];
    pthread_t t[p];
    
    int i,e;
    
  // creazione di p thread ognuno con un range di numeri
  for(i=0;i<p;i++) {
    argomenti[i].inizio = i*(n/p);
    argomenti[i].fine =  (i==p-1) ? n : (i+1)*(n/p);
    argomenti[i].arr = a;
    e = pthread_create(&t[i], NULL, tbody, (void *) &argomenti[i]);
    assert(e==0);
  }
  fprintf(stderr,"%d thread creati\n", p);

  int max = a[0];  
  for(i=0;i<p;i++) {
    e = pthread_join(t[i], NULL);
    if(e==0) {
      fprintf(stderr,"Thread %d terminato\n",i);
	if(max < argomenti[i].max) max = argomenti[i].max;
    }
    else 
      fprintf(stderr,"Errore join %d\n",e);
  }
    return max;
}

// genera array di n elementi con interi random tra -100000 e 100000
int *random_array(int n)
{
  assert(n>0);
  int *a = malloc(n* sizeof(int));
  assert(a!=NULL);
  for(int i=0; i < n ;i++)
    a[i] = (random()%200001) - 100000;
  return a;
}
