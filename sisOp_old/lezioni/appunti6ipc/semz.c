#include "semfun.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define N 5

int semid;

int waitz(int semid, int semnum)
        /* attesa del valore zero sulla componente semnum di semid */
{
  struct sembuf sb;

  sb.sem_num=semnum;
  sb.sem_op=0;
  sb.sem_flg=0;
  return semop(semid,&sb,1);
}

void proc(int i)
{ 
  sleep(i);
  printf("Processo %d con pid %d al termine della prima sezione\n",i,getpid());
  down(semid,0);
  waitz(semid,0);
  printf("Processo %d con pid %d all'inizio della seconda sezione\n",i,getpid());
}

int main()
{
  int val,pid,i;

  semid = semget(IPC_PRIVATE,1,0600);
  printf("Valore semid: %d\n",semid);
  if (semid == -1) perror("Creazione semaforo");
  seminit(semid,0,N);
  val = semctl(semid,0,GETVAL);
  if (val==-1) perror("Lettura valore semaforo");
  printf("Valore semaforo %d: %d\n",semid,val);

  for(i=0;i<N;i++)
        if (fork()==0)
                { proc(i); exit(0);};
  for(i=0;i<N;i++)
        { pid=wait(0);
          printf("Terminato processo %d\n",pid);
        }
  if (semctl(semid,0,IPC_RMID)==-1) perror("Rimozione semaforo");
}