#define _GNU_SOURCE
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SIZE 20


int main()
{
  int *a, *b, n, shmid;
  a = (int *) malloc(SIZE*sizeof(int));

  shmid = shmget(IPC_PRIVATE,SIZE*sizeof(int),0600);
  if (shmid == -1) perror("Creazione memoria condivisa");
  printf("Memoria condivisa con id: %d\n",shmid);
  b = shmat(shmid, 0, 0);

  n = fork();
  if(n==-1) perror("Fork fallita");
  // entrambi i processi scrivono sulla memoria condivisa b[] e su a[] (non condiviso)
  for(int i=0;i<SIZE;i++)
    {a[i] = b[i] = n; sleep(1);}
  // entrambi calcolano le somme  
  int sum=0, zum=0;
  for(int i=0;i<SIZE;i++) {
    sum += a[i];
    zum += b[i];
  }
  printf("Pid=%d Somma privata=%d Somma condivisa=%d\n",getpid(),sum,zum);
  // il padre attende la terminazione del figlio e rimuove la shred memory  
  if(n!=0) {
    wait(NULL);
    if (shmctl(shmid,IPC_RMID,0)==-1) perror("Rimozione shm");
  }
  
}

