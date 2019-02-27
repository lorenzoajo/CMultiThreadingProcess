#define _GNU_SOURCE
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>       
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>


// Mostra varabili e semfori condivisi creati da crypto_producer


int main(int argc, char *argv[])
{

  int key = MYKEY;
  // ottiene memoria condivisa per cindex e tot_lines 
  int shmid = shmget(key + 1, 2*sizeof(int), 0600); 
  if(shmid == -1) perror("shmget"); 
  else {
    int *cindex = (int *) shmat(shmid,NULL,0);
    int *tot_lines = cindex + 1;
    printf("Current cindex: %d\n", *cindex);
    printf("Current tot_lines: %d\n", *tot_lines);
  }
  
  // legge il valore dei semafori
  key = MYKEY; // ri-inizializza key
  int freeslot = semget(key++, 1, 0600);
  if(freeslot == -1) perror("freeslot semget");
  else printf("freeslot current value: %d\n", semctl(freeslot,0,GETVAL,NULL));

  int ready = semget(key++, 1, 0600);
  if(ready == -1) perror("ready semget");
  else printf("ready current value: %d\n", semctl(ready,0,GETVAL,NULL));
  
  int mutex = semget(key++, 1, 0600);
  if(mutex == -1) perror("mutex semget");
  else printf("mutex current value: %d\n", semctl(mutex,0,GETVAL,NULL));
  
  return 0;
}

