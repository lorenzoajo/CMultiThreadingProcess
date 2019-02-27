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
#include "xerrors.h"

// prototipi di funzione
int *random_array(int n);
int intcmp(const void *a, const void *b);
void array_sort(int *a, int n);
bool check_sort(int *a, int n);

// variabili globali utilizzate dal mail e dal signal handler 
volatile bool aspetta_figlio = true; 

// funzione che viene invocata quando viene ricevuto un segnale USR1
void handler(int s)
{

  printf("Segnale %d ricevuto dal processo %d\n", s, getpid());
  if(s==SIGUSR2) {
    aspetta_figlio = false;
  } 
}


// main
int main(int argc, char **argv)
{
  if(argc!= 2)
  {
    fprintf(stderr,"Uso: %s n\n",argv[0]);
    exit(1);
  }
  int n= atoi(argv[1]);
  assert(n>0);
  
   // definisce signal handler 
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);     // setta a "insieme vuoto" sa.sa_mask maschera di segnali da bloccare 
  sa.sa_flags = SA_RESTART;     // restart system calls  if interrupted
  //sigaction(SIGUSR1,&sa,NULL);  // handler per USR1
  sigaction(SIGUSR2,&sa,NULL);  // handler per USR2
  //sigaction(SIGINT,&sa,NULL);   // handler per Control-C
  
  int e, shmid;
  pid_t pid, pid_father;
  pid_father = getpid();	//così salvo il pid del padre in questa varaible che vedra anche il figlio, in modo che avra il suo pid per mandargli segnali quando termina
  // creazione array di memoria condivisa 
  shmid = shmget(IPC_PRIVATE,n*sizeof(int),0600); 
  if(shmid == -1) { perror("shared memory allocation"); exit(1);}
  int *a = shmat(shmid,NULL,0); // ora a punta all'array di memoria condivisa
  // visualizza pid per controllo memoria condivisa
  printf("Il pid di questo processo e': %d, l'id della memoria: %d\n",getpid(),shmid);  
  // creazione array random
  int *arrayrandom = random_array(n);
  for(int i = 0; i<n;i++){
  		a[i]= arrayrandom[i];
  }
  // creazione processo figlio
  pid= xfork(__LINE__, __FILE__);
  // codice processo figlio
  if(pid==0)
  {
    array_sort(&a[n/2], n-n/2);
    
    /* //controllo ordine figlio
     int i;
  	for(i=n/2; i<n;i++) { printf("FIGLIO>>%d:%d\n",i,a[i]);}
  	 sleep(1);
	*/
	
    // detach memoria condivisa  
    e = shmdt(a);
    if(e<0) {perror("shmdt errore:"); exit(1);}
  	else{perror("FIGLIO>>detach memoria ok\n");}
  	
  	//figlio manda segnale SIGUSR2 al padre per far sapere che ha finito
  	kill(pid_father,SIGUSR2);
  	
    exit(0);
  }
  // codice processo padre
  array_sort(a, n/2);

  // aspetta che abbia finito il figlio
  //pid  = xwait(NULL,__LINE__,__FILE__);
  //aspetta il segnale del figlio che gli dice che ha terminato
  while(aspetta_figlio){
  	pause();
  }
  
  //merge dei due vettori
  int x = 0, y = n/2, vet[n], cur = 0;
  while(x<n/2 && y<n){
  	if(a[x]<a[y]){
  		vet[cur++]= a[x++];
  	}
  	else{
  		vet[cur++]= a[y++];
  	}
  }
	while(y<n){
		vet[cur++]= a[y++];
	}
	 while(x<n/2){
		vet[cur++]= a[x++];
	}
  
  
  int i;
  for(i=0; i<n;i++) { printf("%d:%d\n",i,vet[i]); }
  // ora l'array e' ordinato  
  puts(check_sort(vet,n)? "ok\n" : "errore ordinamento\n");
  // detach e rimozione memoria condivisa
  e = shmdt(a);
  if(e<0) {perror("shmdt errore:"); exit(1);}
  else{perror("PADRE>>detach memoria ok\n");}
  e = shmctl(shmid, IPC_RMID,  NULL);
  if(e == -1) {perror("shmctl"); exit(1);}
  return 0;
}


// genera array di n elementi con interi random tra 0 e RAND_MAX
int *random_array(int n)
{
  assert(n>0);
  int *a = malloc(n* sizeof(int));
  assert(a!=NULL);
  for(int i=0;i < n;i++)
    a[i] = (int) random();
  return a;
}

// funzione di confronto tra interi passata da array_sort a qsort
int intcmp(const void *a, const void *b)
{
  return *((int *) a) - *((int *) b);
}
// esegue il sort dell'array a[0..n] utilizzando la funzione di libreria qsort
void array_sort(int *a, int n)
{
  qsort(a,n,sizeof(int), intcmp);
}

// verifica che array a[0..n] sia ordinato 
bool check_sort(int *a, int n)
{
  for(int i=0; i < n-1; i++)
     if(a[i] > a[i+1]) return false;
  return true;
}
