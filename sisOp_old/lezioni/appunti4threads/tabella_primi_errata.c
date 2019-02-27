/*
 * Programma NON corretto per il calcolo multithread 
 * della tabella dei numeri primi fino ad n
 * */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>      
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
// dimensione tabella che contiene i primi
# define TAB_SIZE 10000000

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
  int inizio;// input
  int fine;
  int quanti; // output
  long somma;
  int *tabella_primi; 
  int *pnum;
} dati;


// funzione eseguita dal thread
void *tbody(void *a)
{  
  dati *arg = (dati *)a; 
  fprintf(stderr,"Thread cerca da %d a %d\n",arg->inizio,arg->fine);
  arg->quanti = 0;
  arg->somma = 0;
  for (int j=arg->inizio;j<arg->fine;j++)
    if(primo(j)) {
      arg->quanti++;
      arg->somma += j;
      // le quattro righe seguenti accedono a variabili condivise tra i thread
      // non avendo nessun meccanismo di sincronizzazione causano una race condition 
      int pos = *(arg->pnum);
      if(pos>=TAB_SIZE)  {fprintf(stderr, "Tabella piena\n"); pthread_exit(NULL);} 
      arg->tabella_primi[pos] = j;
      *(arg->pnum) = pos + 1;
    }
  pthread_exit(NULL); 
}

int main(int argc, char *argv[])
{
  // leggi input
  if(argc!=3) {
    printf("Uso\n\t%s limite numthread\n", argv[0]);
    exit(1);
  }
  int n = atoi(argv[1]);
  int p = atoi(argv[2]);
  assert(n>10 && p>0);
  int i,e;
  pthread_t t[p];
  dati argomenti[p];
  
  // alloca la tabella dei primi
  int *tabella = (int *) malloc(TAB_SIZE*sizeof(int));
  assert(tabella!=NULL);
  int messi = 0;

  // creazione di p thread con il loro range di numeri
  for(i=0;i<p;i++) {
    argomenti[i].inizio = i*(n/p);
    argomenti[i].fine =  (i==p-1) ? n : (i+1)*(n/p);
    argomenti[i].tabella_primi = tabella;
    argomenti[i].pnum = &messi;
    pthread_create(&t[i], NULL, tbody, (void *) &argomenti[i]);
  }
  fprintf(stderr,"%d thread creati\n", p);
  
  // attende terminazione thread e calcola somma dei primi
  int quanti=0;
  long somma = 0;
  for(i=0;i<p;i++) {
    e = pthread_join(t[i], NULL);
    if(e==0) {
      fprintf(stderr,"Thread %d terminato\n",i);
      quanti += argomenti[i].quanti;
      somma += argomenti[i].somma;
    }
    else 
      fprintf(stderr,"Errore join %d\n",e);
  }
  fprintf(stderr,"Trovati %d primi con somma %ld\n",quanti,somma);
 
  // ricalcola la somma utilizzando i primi nella tabella
  // se si usa piu' di un thread il risultato non e' corretto 
  // in quanto vengono persi dei primi a causa della race
  // maggiore e' il numero di thread maggiore e' il numero di primi persi
  somma = 0;
  for(i=0;i<messi;i++) somma += tabella[i];
  fprintf(stderr, "Nella tabella ci sono %d primi con somma %ld\n",messi,somma);
}
