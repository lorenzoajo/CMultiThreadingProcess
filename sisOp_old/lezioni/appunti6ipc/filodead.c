#include "semfun.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>


/* 5 filosofi con possibile deadlock */

int semid,shmid;
char *stato; /* puntera' ad un array di 5 caratteri in 
	        memoria condivisa che serve solo per illustrare
		 all'utente lo stato dei 5 filosofi, 
		T=thinking, H=hungry, E=eating */

void cleanup(int sig)
{
  if (semctl(semid,0,IPC_RMID) == -1)
     perror("semctl");
  if (shmctl(shmid,IPC_RMID,0) == -1)
     perror("semctl");
  exit(sig);
}

void print()	/* stampa lo stato dei filosofi */ 
{
  int i;
  for(i=0;i<5;i++) putchar(stato[i]);
  printf("\n");
}

void proc(int i)
{
  int j,n;
  int m;

  m = 50000000 + 30000000 * i; /* tempo per cui usera' le forchette; 
	i filosofi con i piu' alto sono piu' lenti a mangiare, questo differenzia
	un pochino il loro comportamento e su qualche sistema favorisce 
	il verificarsi del deadlock */

  for (n=0;n<50;n++)
  {
    stato[i]='H';
    printf("FILOSOFO %d PRENDE LA %d  ",i,i);
    print();
    down(semid,i);
    down(semid,(i+1)%5);
    stato[i]='E';
    printf("FILOSOFO %d MANGIA       ",i);
    print();
    for(j=0;j<m;j++); /* ogni processo usa le risorse per un tempo diverso */
    stato[i]='T';
    printf("FILOSOFO %d SMETTE       ",i);
    print();
    up(semid,(i+1)%5);
    up(semid,i);
  }
}

int main()
{
  int i,j;
  pid_t pid;
  struct sigaction sa;

  if ((semid = semget(IPC_PRIVATE,5,0600))==-1)
     perror("semget");
  if ((shmid = shmget(IPC_PRIVATE,5,0600))==-1)
     perror("shmget");
  stato = (char *) shmat(shmid,0,0);
  if ((int)stato == -1) perror("shmat");
  for (i=0;i<5;i++)
  {
    stato[i]='T';
    seminit(semid,i,1);
  }

  for(i=0;i<5;i++)
	{
	if (fork()==0)
		{ proc(i); exit(0);}
	}

  sa.sa_handler = cleanup;
  sigaction(SIGINT,&sa,NULL);
     /* se vanno in deadlock, bisogna interrompere tutto,
	 il processo padre associa cleanup a SIGINT per rimuovere 
	semafori e memoria condivisa in questo caso */

  for(i=0;i<5;i++)
	{ 
	  pid=wait(0);
	  printf("Terminato processo %d\n",pid);
	}
  cleanup(0);

}