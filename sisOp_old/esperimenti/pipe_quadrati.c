
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
#include <sys/wait.h>

//meccanismo pipeup e pipedown

int main()
{

	int pipe_fd[2];
	
	pipe(pipe_fd);
	
	
	//pipe con la quale i figli comunicheranno il risultato al padre
	int pipe_up[2];
	pipe(pipe_up);
	
	
	int e,i;
	pid_t pid_figli[2];
	
	
	for(i=0;i<2;i++){
		pid_figli[i] = fork();
		if(pid_figli[i]==-1) exit(1);
		if(pid_figli[i]==0){
			//codice figli
			close(pipe_fd[1]);
			close(pipe_up[0]);
			int sum = 0, a;
			pid_t pid = getpid();
			char buff[100];
			FILE *fin = fdopen(pipe_fd[0],"r");
			assert(fin != NULL);
			while(fgets(buff,sizeof(buff),fin)){
				a= atoi(buff);
				sum +=a;
				printf("il valore letto da %d e' %d\n",pid,a);
			}
			
			//scrive il risultato al padre tramite pipe
			close(pipe_fd[0]);
			

			
			FILE *fout = fdopen(pipe_up[1],"a");
			fprintf(fout,"%d\n",sum);
			fclose(fout);
			close(pipe_up[1]);
			
			
			exit(0);
		}
	
	}
	
	//codice padre
	close(pipe_fd[0]);
	close(pipe_up[1]);
	int j;
	char buff[10];
	FILE *fout = fdopen(pipe_fd[1],"a");
	for(j=0;j<100;j++){
		fprintf(fout,"%d\n",j);
	}
	fclose(fout);
	close(pipe_fd[1]);	//fa arrivare EOF a chi leggeva
	
	
	wait(NULL);

	//il padre legge il risultato dei figli
	FILE *fin = fdopen(pipe_up[0],"r");
	assert(fin != NULL);
	int sum = 0,a;
	while(fgets(buff,sizeof(buff),fin)!=NULL){
		a= atoi(buff);
		sum +=a;
	}
	printf("la somma dei figli e' %d \n",sum);
	fclose(fin);
	close(pipe_up[0]);
	
	
	return 0;
}     











