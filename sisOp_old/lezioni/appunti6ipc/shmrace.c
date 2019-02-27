
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int main(int argc,char *argv[])
{
  int j,k,n,shmid,*addr;

  if (argc != 2) {fprintf(stderr,"Passare un numero come argomento\n");exit(1);}
  k = atoi(argv[1]);

  shmid = shmget(IPC_PRIVATE,sizeof(int),0600);
  if (shmid == -1) perror("Creazione memoria condivisa");
  addr = (int *) shmat(shmid, NULL, 0);
  if (addr == (void *)-1) perror("Attach memoria condivisa");
  *addr = 10; /* inizializza il dato condiviso */

  n= fork();
  if (n == -1) perror("Fork");

  if (n == 0)
  {
    for (j=0;j<k;j++) (*addr)++;
  }
  else
  {
    for (j=0;j<k;j++) (*addr)--;

    wait(NULL);
    printf("Valore numero condiviso: %d\n", *addr);
    if (shmctl(shmid,IPC_RMID,0)==-1) perror("Rimozione shm");
  }
}


