#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/* verifica gestione coda semafori */

sem_t sem; /* creo semaforo posix*/


void *tf(void *p)
{
  
  printf("Thread %d prima della sem_wait\n",*(int *)p);
  sem_wait(&sem);
  printf("Thread %d dopo la sem_wait\n",*(int *)p);
}

int main()
{
int i,j;
pid_t pid;
pthread_t t[5];


/* inizializzo semaforo sem a 0 (setta "rosso") */
if(sem_init(&sem,0,0)==-1) {perror("sem_init"); exit(0);} 


for(i=0;i<5;i++)
	 { pthread_create(&t[i], NULL, tf, (void *)&i);sleep(1);}

for(i=0;i<5;i++) 
	{sem_post(&sem);
	sleep(1);
	}
for(i=0;i<5;i++)
	{ pthread_join(t[i], NULL);
	  printf("Terminato thread %d\n",i);
	}
sem_destroy(&sem);	
}
