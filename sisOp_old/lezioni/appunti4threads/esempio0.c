// semplice esempio di programma che lancia un singolo thread
// per compilare questo e gli esempi successivi usare 
// l'opzione di compilazione -pthread:
//   gcc -pthread -Wall -std=c99 -o esempio0 esempio0.c
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>      
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>


// funzione eseguita dal thread
void *tbody(void *a)
{
  fprintf(stderr,"Thread 2, pid:%d\n",getpid());
  int *arg = (int *)a;
  *arg = 10;   // pone uguale a 10 il parametro passato attraverso *a 
  sleep(4);    // mostra che la join attende la terminazione 
  pthread_exit(NULL);         /* oppure return(NULL); */
}


int main(int argc, char *argv[])
{
  int i=0,e;
  pthread_t t;

  fprintf(stderr,"Thread principale, pid: %d\n",getpid());

  // creazione di un thread passandogli il parametro i=0
  pthread_create(&t, NULL, tbody, (void *) &i);
  
  // attesa terminazione thread
  e = pthread_join(t, NULL);
  if(e==0)
    fprintf(stderr,"Thread terminato. i ora vale: %d\n",i);
  else
    fprintf(stderr,"Errore join %d\n",e);
}
