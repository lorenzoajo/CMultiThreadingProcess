/*
 * Compito del 18/7/17
 * Esempio di 3 thread che comunicano attraverso
 * due diversi producer/consumer buffer
 * interrompendosi quando viene ricevuto il segnale USR1
 * 
 * Per mandare il segnale USR1 al programma 
 *   kill -usr1 <pid>
 * */
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

// dimensione dei buffer produttori/consumatori
#define Buf_size 10


// struct contenente i parametri di input e output di ogni thread 
typedef struct {
  char **buf;           // buffer
  sem_t *free, *ready;  // semafori per gestione del bufer
  FILE *f;              // usato solo da t3
} dati;


// body del thread solo consumatore
void *t3(void *arg)
{  
  dati *a = (dati *)arg; 
  int e;
  int i = 0;
  
  while(1) {
      e = sem_wait(a->ready);
      assert(e==0);
      char *s = a->buf[i++ % Buf_size];
      e = sem_post(a->free); // segnala spazio disponibile sul buffer
      assert(e==0);
      if(s==NULL) break;
      fputs(s,a->f);        // scrive stringa su file 
      free(s);              // dalloca stringa
  }
  fprintf(stderr,"Thread 3 terminato. Righe scritte: %d\n",i-1);
  return NULL;
}  
    
  
// body del thread consumatore e produttore
void *t2(void *arg)
{  
  dati *a = (dati *)arg; 
  int e;
  int i = 0, j=0;
  // start execttion of the t3 thread
  pthread_t t;
  dati b;
  char *buf[Buf_size];
  sem_t free, ready;
  sem_init(&free,0,Buf_size);
  sem_init(&ready,0,0);
  b.free = &free;
  b.ready = &ready;
  b.buf = buf;
  b.f = a->f;
  pthread_create(&t,NULL,t3,(void *) &b);
  while(1) {
      e = sem_wait(a->ready);// aspetta dati sul buffer
      assert(e==0);
      char *s = a->buf[i++ % Buf_size];
      e = sem_post(a->free); // segnala spazio disponibile sul buffer
      assert(e==0);
      // esegue la conversionr
      if(s!=NULL) {
        for(int k=0;k<strlen(s);k++) {
          if(isupper(s[k])) s[k] = tolower(s[k]);
          else if(islower(s[k])) s[k] = toupper(s[k]);
        }
      }
      // scrive su buffer per t3
      e = sem_wait(&free);
      assert(e==0);
      buf[j++ % Buf_size] = s;
      e = sem_post(&ready); // segnala dati disponibili sul buffer
      assert(e==0);
      if(s==NULL) break;
  }
  fprintf(stderr,"Thread 2 terminato. Righe scritte: %d\n",i-1);
  e = pthread_join(t, NULL);
  if(e!=0) fprintf(stderr,"Errore join %d\n",e);
  e = sem_destroy(&free);
  assert(e==0);
  e = sem_destroy(&ready);
  assert(e==0);
  return 0;
}  


// variabile globale e signal handler 
bool fermati_ora = false;

void fermati(int sig)
{
  fermati_ora = true;
  puts("Segnale ricevuto");
}


// main legge linee dal file e le passa a t2
int main(int argc, char *argv[])
{
  int e,i=0;

  // leggi input
  if(argc!=3) {
    fprintf(stderr,"Uso\n\t%s filein fileout\n", argv[0]);
    exit(1);
  }
  // stampa pid per inviare segnale
  fprintf(stderr,"Pid: %d\n",getpid());
  // creo handler per SIGUSR1 
  struct sigaction sa;
  sa.sa_handler = fermati;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGUSR1,&sa,NULL);
  
  // open input & output file 
  FILE *f = fopen(argv[1],"r");
  if(f==NULL) {perror("Errore apertura file"); return 1;}
  FILE *g = fopen(argv[2],"w");
  if(g==NULL) {perror("Errore apertura file"); return 1;}

  // creazione thread t2
  pthread_t t;
  dati a;
  sem_t sfree, ready;
  e=sem_init(&sfree,0,Buf_size);
  assert(e==0);
  e=sem_init(&ready,0,0);
  assert(e==0);
  char *buf[Buf_size];
  a.buf = buf;
  a.free = &sfree;
  a.ready = &ready;
  a.f = g;
  pthread_create(&t, NULL, t2, (void *) &a);
  char *s; size_t slen;
  // --- legge righe
  while(true) {
    sleep(2);      // rallenta esecuzione per testare l'uso del segnale
    s = NULL;      
    int j = getline(&s,&slen,f); // getline provvede a allocare il buffer 
    fprintf(stderr,"Lunghezza riga: %zu\n",strlen(s));
    if(j<0 || fermati_ora) {
      if(s) free(s);             
      s=NULL;                    // write NULL to buffer to stop consumer 
    }
    e = sem_wait(&sfree);        // wait till there is a free slot
    assert(e==0);
    buf[i++ % Buf_size] = s;     // write pointer to the line to be converted
    e = sem_post(&ready);        // one more data available
    assert(e==0);
    if(j == -1 || fermati_ora) break;
  }
  fprintf(stderr,"Righe lette dal file di input: %d\n",i-1);  
  fclose(f);

  // cleanup threads
  e = pthread_join(t, NULL);
  if(e!=0) fprintf(stderr,"Errore join (main)%d\n",e);
  e = sem_destroy(&sfree);
  assert(e==0);
  e = sem_destroy(&ready);
  assert(e==0);
  fclose(g);
  return 0;
}

