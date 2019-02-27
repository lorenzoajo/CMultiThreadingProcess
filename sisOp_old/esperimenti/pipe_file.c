#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


int main(int argc, char**argv){
	
	if(argc < 2){
		perror("inserisci nomi file\n");
		exit(1);
	}
	
	
	int pipefd[2];
	if(pipe(pipefd)){
		perror("errore pipe\n");
		exit(2);	
	}


	pid_t pid_child = fork();
	if(pid_child == -1){
			perror("errore fork\n");
			exit(3);
	}
	
	
	if(pid_child){
		//codice padre
		close(pipefd[0]);	//chiudo la parte di lettura perchè il padre scrive
		
		char buff[100];
		FILE* fin = fopen(argv[1],"r");
		while(fgets(buff,sizeof(buff),fin)){
			write(pipefd[1],buff,sizeof(buff));
			
		}
		close(pipefd[1]);
		
	}
	else{
		//codice figlio
		close(pipefd[1]);	//chiudo la parte di scrittura perchè il figlio legge
		
		
		FILE* fout = fopen("output.txt","w");
		char buff[100];
		while(read(pipefd[0],buff,sizeof(buff))){
			fprintf(fout,"%s",buff);
		}
		
		exit(1);
		
	}

	wait(NULL);	
	
	return 0;
}
