/********  N produttori, 1 consumatore con processi e semafori ********/

#include "semfun.h"

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define N 5    		 /* dim. buffer */
#define NPROD 5    	/* numero produttori */
#define NGIRI 20 	/* quanti caratteri prodotti da ogni produttore */

struct buffer{
        char pool[N];
        int in,out,count;
        } *buf;		/* puntatore al buffer che viene allocato 
			   in memoria condivisa */

int shmid,mutex,empty,full;

void printstatus()	/* stampa contenuto del buffer */
{
  int i;
 
  printf("Contenuto del buffer:");
  for (i=0;i<buf->count;i++)
      putchar(buf->pool[(buf->out+i)%N]);
}

void put(char i)
{
 
  buf->pool[buf->in]=i;
  buf->in = (buf->in+1)%N;
  buf->count++;
 
  printstatus();
  printf(" dopo put del carattere %c\n",i);
  fflush(stdout); /* cosi' va bene anche se si ridirige l'output su disco */
}

void get(char *ip)
{

  *ip=buf->pool[buf->out];
  buf->out = (buf->out+1)%N;
  buf->count--;
  
  printstatus();
  printf(" dopo get del carattere %c\n",*ip);
  fflush(stdout); /* cosi' va bene anche se si ridirige l'output su disco */
}
 

void producer(char c)	/* produce NGIRI volte il carattere c */
{
  int i;
 
  for (i=0;i<NGIRI;i++)
        {
 
	int j;
        for (j=0;j<10000000;j++); /* per rallentare */

	down(empty,0);
        down(mutex,0);
        put(c);
        up(mutex,0);
	up(full,0);
        }
}

void consumer(char *filename)	/* copia da buffer a filename */
{
  char c;
  FILE *res;
 
  res=fopen(filename,"w");

  do /* esce quando trova il carattere \0 nel buffer */
  {
  	int i;
 
  	for (i=0;i<10000000;i++); /* per rallentare */
 
  	down(full,0);
  	down(mutex,0);
  	 get(&c);
  	up(mutex,0);
  	 up(empty,0);
	
  	putc(c,res);
  }
  while (c!=0);
}
 

void clear(int s)	/* rimuove strutture IPC */
{

if (semctl(mutex,0,IPC_RMID) == -1) perror("semctl");
if (semctl(full,0,IPC_RMID) == -1) perror("semctl");
if (semctl(empty,0,IPC_RMID) == -1) perror("semctl");
if (shmctl(shmid,IPC_RMID,0) == -1) perror("shmctl");
exit(s);
}
 
int main(int argc, char *argv[])
{

int i,pid;
struct sigaction sa;

if (argc != 2) 
{
	fprintf(stderr,"un nome di file come argomento\n");
	 exit(1);
}

if ((mutex = semget(IPC_PRIVATE,1,0666))==-1)
     perror("semget");
seminit(mutex,0,1);

if ((empty = semget(IPC_PRIVATE,1,0666))==-1)
     perror("semget");
seminit(empty,0,N);
 
if ((full = semget(IPC_PRIVATE,1,0666))==-1)
     perror("semget");
seminit(full,0,0);
 
 
/* acquisisce memoria condivisa per il buffer */
 
if ((shmid = shmget(IPC_PRIVATE,sizeof(struct buffer),0666))==-1)
     perror("shmget");
buf = (struct buffer *) shmat(shmid,0,0);
if ((int) buf == -1) perror("shmat");
 
 sa.sa_handler = clear;
sigaction(SIGINT,&sa,NULL);

/* inizializza buffer */
 
buf->in = buf->out = buf->count = 0;
 
/* genera produttori e consumatore */

 
fflush(stdout);


for(i=0;i<NPROD;i++)
	{
	if (fork()==0)
		{ producer('0'+i); exit(0);}
	}
if (fork()==0)
	{ consumer(argv[1]); exit(0);}


for(i=0;i<NPROD;i++)
	{ pid=wait(NULL);
	  printf("Terminato processo %d\n",pid);
	}

/* produce carattere 0 per far terminare il consumatore */
down(empty,0);
down(mutex,0);
put(0);
up(mutex,0);
up(full,0);

wait(NULL);
clear(0);
}