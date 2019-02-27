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

#include "semfun.h"

// producer for brute force password attack
 

int main(int argc, char *argv[])
{

  if(argc!=3) {
    fprintf(stderr,"USAGE:  %s encrypted_pwd  dictionary\n\n",argv[0]);
    return 1;
  }
  // controlla se il file dizionario esiste
  FILE *f = fopen(argv[2],"r");
  if(f==NULL) {perror(argv[2]); return 2;}
  
  // creazione memoria e semafori condivisi
  int key = MYKEY;
  int buf_size = BUFSIZE;
  int pwd_size = PWDSIZE;

  // crea memoria condivisa per il buffer delle stringhe da analizzare
  int shmid = shmget(key++, buf_size*pwd_size*sizeof(char), 0600|IPC_CREAT|IPC_EXCL); 
  if(shmid == -1) {perror("shmget");return 3;}
  char *buffer = (char *) shmat(shmid,NULL,0);

  // crea memoria condivisa per cindex e tot_lines 
  shmid = shmget(key++, 2*sizeof(int), 0600|IPC_CREAT|IPC_EXCL); 
  if(shmid == -1) {perror("shmget");return 3;}
  int *cindex = (int *) shmat(shmid,NULL,0);
  int *tot_lines = cindex + 1;
  
  // crea due semafori per la gestione del buffer piu' un terzo da usare come mutex 
  key = MYKEY; // ri-inizializza key
  int freeslot = semget(key++, 1, 0600 | IPC_CREAT | IPC_EXCL);
  if(freeslot == -1) { perror("semget"); return 4;}
  int ready = semget(key++, 1, 0600 | IPC_CREAT | IPC_EXCL);
  if(ready == -1) { perror("semget"); return 4;}
  int mutex = semget(key++, 1, 0600 | IPC_CREAT | IPC_EXCL);
  if(mutex == -1) { perror("semget"); return 4;}
  // inizializza i tre semafori
  seminit(freeslot,0,buf_size); // free_slot
  seminit(ready,0,0);        // ready items 
  seminit(mutex,0,1);        // mutex 
  
  // loop principale di lettura dal file
  *tot_lines = 0; 
  int pindex = *cindex = 0;   // inizializza indici nel buffer di producer e consumer
  while(1) {
    down(freeslot,0);     // attende ci sia uno freeslot
    int offset = (pindex++ % buf_size); 
    char *c = fgets(buffer + (offset*pwd_size), pwd_size,f);
    if(c==NULL) break;
    up(ready,0);
    *tot_lines += 1;
  }
  puts("End of file. The server will be terminated");
  return 0;
}

