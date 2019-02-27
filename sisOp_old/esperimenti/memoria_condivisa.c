#define _GNU_SOURCE
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
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

  if(n!=0){
    //codice padre
    int i = 0;
    for( i = 0;i<SIZE;i++){
      b[i] = i;
    }

    // il padre attende la terminazione del figlio e rimuove la shred memory
    wait(NULL);
    printf("padre>> la somma è: %d\n",b[0]);
    if (shmctl(shmid,IPC_RMID,0)==-1) perror("Rimozione shm");

  }
  else{
    //codice figlio
    int sum = 0;
    int i = 0;
    for( i = 0;i<SIZE;i++){
      sum += b[i];
    }
    printf("figlio>> la somma è: %d\n",sum);
    b[0]=sum; //restituisco nel primo punto del vettore sum
    exit(1);
  }


  return 0;
}
