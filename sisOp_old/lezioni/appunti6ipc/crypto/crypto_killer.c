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


// dealloca la shared memory e i semafori creati da crypto_server 


int main(int argc, char *argv[])
{

  
  // eliminazione memoria e semafori condivisi
  int e, key = MYKEY;
  int buf_size = BUFSIZE;
  int pwd_size = PWDSIZE;  
  // elimina memoria condivisa per il buffer delle stringhe da analizzare
  int shmid = shmget(key++, buf_size*pwd_size*sizeof(char), 0600); 
  if(shmid == -1) perror("shmget 1");
  else {
    e = shmctl(shmid, IPC_RMID, NULL);
    if(e==-1) perror("shmctl 1");
    else puts("shm delete OK");
  }
  
  // elimina memoria condivisa per cindex e tot_lines 
  shmid = shmget(key++, 2*sizeof(int), 0600); 
  if(shmid == -1) perror("shmget 2");
  else {
    e = shmctl(shmid, IPC_RMID, NULL);
    if(e==-1) perror("shmctl 2");
    else puts("shm delete OK");
  }
  
  // eliminazione due semafori per la gestione del buffer piu' un terzo da usare come mutex
  key = MYKEY; // ri-inizializza key
  for(int i=0;i<3;i++) {
    int sem = semget(key++, 1, 0600);
    if(sem == -1) perror("semget");
    else {
      e =  semctl(sem,0,IPC_RMID);
      if(e==-1) perror("semctl");
      else puts("sem delete OK");
    }
  }
  puts("Done");
  return 0;
}

