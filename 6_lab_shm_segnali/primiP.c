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

bool primo(int n)
{
  int i;
  if(n<2) return false;
  if(n%2==0) {
    if(n==2)
      return true;
    else
      return false;
  }
  for (i=3; i*i<=n; i += 2) {
      if(n%i==0) {
          return false;
      }
  }
  return true;
}


// variabili globali utilizzate dal mail e dal signal handler 
volatile int aspetta_N_figlii = 0; 

// funzione che viene invocata quando viene ricevuto un segnale USR1
void handler(int s)
{

  printf("Segnale %d ricevuto dal processo\n", s);
  if(s==SIGRTMIN) {
    aspetta_N_figlii ++;
  } 
}

int main(int argc, char **argv){
	
	if(argc != 3){
    	fprintf(stderr,"Uso: %s M N\n",argv[0]);
    	exit(1);
	}
	
	int M = atoi(argv[1]);	//numero a cui fermarsi
	int N = atoi(argv[2]);	//numero di processi
	
	// definisce signal handler 
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);     // setta a "insieme vuoto" sa.sa_mask maschera di segnali da bloccare 
	sa.sa_flags = SA_RESTART;     // restart system calls  if interrupted
	sigaction(SIGRTMIN,&sa,NULL);  // handler per USR1

	pid_t pid_father = getpid();
	
	//allocazione memoria per restituire risultato
	int e, shmid;
	shmid = shmget(IPC_PRIVATE,N*sizeof(int),0600); 
  	if(shmid == -1) { perror("shared memory allocation"); exit(1);}
 	int *a = shmat(shmid,NULL,0); 
	
	for(int i=0;i<N;i++){
		if(!xfork(__LINE__,__FILE__)){
			int cont = 0;
			//codice filgio i-esimo
			for(int j=2*i+1; j<M;j = j+ 2*N){
				if(primo(j)) cont++;
			}
			
			a[i] = cont; //restituisco risultato al padre
			
			e = shmdt(a);
   			if(e<0) {perror("shmdt errore:"); exit(1);}
   			
   			kill(pid_father,SIGRTMIN); //segnala al padre che ha finito
			exit(1);
		}
	}
	
	//codice processo padre
	
	/*
	for(int i=0;i<N;i++){
		xwait(NULL,__LINE__,__FILE__);
	}
	*/
	
	
	//se uso i segnali, non va perchè non ne riceve uno 
	//USO I SEGNALI REAL TIME CHE NON VENGONO ACCORPATI O PERSI
	while(aspetta_N_figlii<N){
		//pause();
		sleep(1);
	}
	
	
	// somma dei risultati e stampa risultato
	int ris =0;
	for(int i=0;i<N;i++){
		ris += a[i];
	}	
	printf("PADRE>> Risultato: %d\n",ris);
	e = shmdt(a);
 	if(e<0) {perror("shmdt errore:"); exit(1);}
 	e = shmctl(shmid, IPC_RMID,  NULL);
  	if(e == -1) {perror("shmctl"); exit(1);}
	return 0;
}






























