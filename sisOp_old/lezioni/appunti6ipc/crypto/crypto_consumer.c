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


// consumer for brute force attack
// compile with -lcrypt


bool test_variants(char *s, char *encrypted);
bool replace_and_test(char *s, int i, char c, char *encrypted);
bool test_single(char *s, char *encrypted);


int main(int argc, char *argv[])
{  
  
  if(argc!=2) {
    fprintf(stderr,"USAGE:  %s encrypted_pwd\n\n",argv[0]);
    return 1;
  }

  // accesso memoria e semafori condivisi
  int key = MYKEY;
  int buf_size = BUFSIZE;
  int pwd_size = PWDSIZE;
  // accede a memoria condivisa per il buffer delle stringhe da analizzare
  int shmid = shmget(key++, buf_size*pwd_size*sizeof(char), 0600); 
  if(shmid == -1) {perror("shmget 1");return 3;}
  char *buffer = (char *) shmat(shmid,NULL,0);
  
  // accede a memoria condivisa per cindex tot_lines 
  shmid = shmget(key++, 2*sizeof(int), 0600); 
  if(shmid == -1) {perror("shmget 2");return 3;}
  int *cindex = (int *) shmat(shmid,NULL,0);
  
  // accede a due semafori per la gestione del buffer piu' un terzo da usare come mutex
  key = MYKEY; // ri-inizializza key
  int freeslot = semget(key++, 1, 0600);
  if(freeslot == -1) { perror("semget 1"); return 4;}
  int ready = semget(key++, 1, 0600);
  if(ready == -1) { perror("semget 2"); return 4;}
  int mutex = semget(key++, 1, 0600);
  if(mutex == -1) { perror("semget 3"); return 4;}

  char s[pwd_size];
  while(1) {
    down(ready,0);     // attende ci sia una unita' di lavoro disponibile
    down(mutex,0);     // usa il semaforo come mutex per *cindex
    int offset = ((*cindex)++ % buf_size);
    strcpy(s,buffer+(offset*pwd_size));
    up(mutex,0);       // fine accesso dati comuni
    up(freeslot,0);    // ho liberato uno slot
    if(test_variants(s, argv[1]))
      break;
  }
  printf("Password found by %d: %s\n", getpid(), s);
  return 0;
}


// verifica se s o una sua variante e' la password 
bool test_variants(char *s, char *encrypted) 
{
  if(s[strlen(s)-1]=='\n') s[strlen(s)-1]=0; 
  if(test_single(s, encrypted)) return true;
  for(int i=0;i<strlen(s);i++) {
    // replace 1 for i
    if(s[i]=='i') 
      if(replace_and_test(s,i,'1',encrypted)) return true;
    // replace @ for a          
    if(s[i]=='a') 
      if(replace_and_test(s,i,'@',encrypted)) return true;
    // replace 0 for o          
    if(s[i]=='o') 
      if(replace_and_test(s,i,'0',encrypted)) return true;
  }
  return false;
}

// prova la password sostituendo s[i] con c 
bool replace_and_test(char *s, int i, char c, char *encrypted)
{
  char o = s[i];
  s[i] = c;
  if(test_single(s,encrypted)) return true;
  s[i] = o;
  return false;
}

// prova se s e' la password
bool test_single(char *s, char *encrypted) {
  char *c = crypt(s, encrypted);
  if(c==NULL) {puts("Error calling crypt"); exit(3);}
  return (strcmp(c,encrypted)==0);
}
