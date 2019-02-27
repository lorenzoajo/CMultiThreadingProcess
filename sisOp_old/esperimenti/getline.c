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
#define TOT_THERAD 4
#define N 10


typedef struct{
	char **buffer;
	int next_in;
	int next_out;
	FILE* fout;
	FILE* fin;
}dati;



int main(int argc, char**argv){
	

	char *buff[30];
	int i = 0, j = 1;
	
	
	if(argc<2){
		perror("file input da riga di comando mancante\n");
		exit(1);
	}
	
	//apertura file input e output
	FILE *fin, *fout;
	fin=NULL;fout=NULL;
	fin = fopen(argv[1],"r");
	fout = fopen("output.txt","w");
	if(fin==NULL || fout==NULL){
		perror("errore fopen\n");
		exit(2);
	}
	

	
	char *s;
	s= NULL;
	size_t slen;
	do{
		j = getline(&s,&slen,fin);
		buff[i] = s;
			//printf("%s\n",s);
		i++;
	}while(j>0);
	
	j=0;
	while(j<i){
		fprintf(fout,"%s",buff[j]);
		j++;
	
	}




	
	fflush(fout);
	
	//chiusura dei file
	fclose(fin);
	fclose(fout);
	return 0;
}




