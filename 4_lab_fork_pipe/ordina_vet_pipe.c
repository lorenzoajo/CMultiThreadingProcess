#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>  
#include <assert.h>   
#include "xerrors.h"

int *random_array(int n)
{
  assert(n>0);
  int *a = malloc(n* sizeof(int));
  assert(a!=NULL);
  for(int i=0;i < n;i++)
    a[i] = (int) random();
  return a;
}

// funzione di confronto tra interi passata da array_sort a qsort
int intcmp(const void *a, const void *b)
{
  return *((int *) a) - *((int *) b);
}
// esegue il sort dell'array a[0..n] utilizzando la funzione di libreria qsort
void array_sort(int *a, int n)
{
  qsort(a,n,sizeof(int), intcmp);
}


// verifica che array a[0..n] sia ordinato 
bool check_sort(int *a, int n)
{
  for(int i=0; i < n-1; i++)
     if(a[i] > a[i+1]) return false;
  return true;
}

int main(int argc, char *argv[])
{
  // leggi input
  if(argc!=2) {
    printf("Uso\n\t%s limite numprocessi\n", argv[0]);
    exit(1);
  }
  int n = atoi(argv[1]);
  assert(n>0);
  
  int *a= random_array(n);
	int i;
  // crea processi
  pid_t pid;
  // crea pipe per la comnicazione dai figli al padre.
  int pipefd[2];
  xpipe(pipefd,__LINE__,__FILE__);
  


  // crea processo figlio
	pid = xfork(__LINE__,__FILE__);
	if (pid==0) {
	  // codice dei processi figli
	  close(pipefd[0]);
	 //ordinamento parte vettore >=RAND_MAX/2  
	  int cont = 0;
	  for(i=0;i<n;i++){
		if(a[i]>=(RAND_MAX/2)) cont++;
	  }
	  int *b= malloc(cont*sizeof(int));
	    int z=0;
		for(i=0;i<n;i++){
			if(a[i]>=(RAND_MAX/2)) b[z++]=a[i];
	  	}
	  	array_sort(b,cont);
	  
	  //passaggio al padre della sua parte di vettore ordinata
	  int ew = write(pipefd[1],b,(sizeof(int)*cont));
	  if(ew !=(sizeof(int)*cont)){
	  		perror("errore write\n");
	  		exit(-1);
	  }
	  free(b);
	  
	  close(pipefd[1]); 
	  exit(0);
	}

  // codice del processo padre
	close(pipefd[1]);
	//ordinamento parte vettore <RAND_MAX/2
  int cont = 0;
  for(i=0;i<n;i++){
	if(a[i]<(RAND_MAX/2)) cont++;
  }
  int *b= malloc(cont*sizeof(int));
  int z=0;
	for(i=0;i<n;i++){
		if(a[i]<(RAND_MAX/2)) b[z++]=a[i];
  	}
   	array_sort(b,cont);
  	
  	
  	int *c = malloc(sizeof(int)*(n-cont));
  	int er = read(pipefd[0],c,sizeof(int)*(n-cont));
	if(er < 0){
	  		perror("errore read\n");
	  		exit(-1);
	  }
	close(pipefd[0]);
	wait(NULL);
	
	//trasferimento nel vecchio vettore
	for(i=0;i<cont;i++){
		a[i]=b[i];
	}
	int y=0;
	for(i=cont;i<n;i++){
		a[i]=c[y++];
	}
	free(b);
	free(c);
 
  if(check_sort(a,n)){
  	printf("tutto fatto correttamente\n");
  }
  else{
  	printf("il vettore non è ordinato\n");
  }

}
