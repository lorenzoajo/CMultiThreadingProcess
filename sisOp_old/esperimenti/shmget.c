#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h> 
#include <signal.h>     
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
	int shmid = shmget(IPC_PRIVATE,20*sizeof(int),0600);
	assert(shmid != -1);
	
	int n = fork();
	assert(n != -1);
	
	if(n==0){
		//codice filgio
		sleep(5);
		int *id1 = shmat(shmid,0,0);
		printf("%d\n",id1[1]);
		
	}
	else{
		//codice padre
		int * id1 = shmat(shmid,0,0);
		id1[1]=2;
	}
	
	shmctl(shmid,IPC_RMID,0);
	
	wait(NULL);
	
	return 0;
}


//con caratteri

/*
int main(int argc, char **argv)
{
	int shmid = shmget(IPC_PRIVATE,20*sizeof(char),0600);
	assert(shmid != -1);
	
	int n = fork();
	assert(n != -1);
	
	if(n==0){
		//codice filgio
		sleep(5);
		char *id1 = shmat(shmid,0,0);
		printf("%s\n",id1);
		
	}
	else{
		//codice padre
		char *id1 = shmat(shmid,0,0);
		sprintf(id1,"ciao\n");
	}
	
	shmctl(shmid,IPC_RMID,0);
	
	wait(NULL);
	
	return 0;
}
*/
