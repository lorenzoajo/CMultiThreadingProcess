#include "semfun.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>

int main()
{
  int semid,val,n;

  semid = semget(IPC_PRIVATE,1,0600);
  if (semid == -1) perror("Creazione semaforo");
  printf("Identificatore semaforo: %d\n",semid);
  seminit(semid,0,0);
  val = semctl(semid,0,GETVAL);
  if (val==-1) perror("Lettura valore semaforo");
  printf("Valore semaforo %d: %d\n",semid,val);
  n= fork();
  if (n == -1) perror("Fork");

  if (n == 0)
  {
    sleep(3);
    printf("Do via libera\n");
    up(semid,0); 
  }
  else
  {
    down(semid,0); 
    printf("Ricevuto via libera\n");
    sleep(3);
    if (semctl(semid,0,IPC_RMID)==-1) perror("Rimozione semaforo");
  }
}