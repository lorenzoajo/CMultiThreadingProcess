#include <stdio.h>    // permette di usare scanf printf .. 
#include <stdlib.h>   // conversioni stringa/numero rand() abs() exit() etc ...
#include <stdbool.h>  // gestisce tipo bool (per variabili booleane)
#include <assert.h>   // permette di usare la funzione assert
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void scriviNn(int n, char *nome) 
{
	// apre file in scrittura
	int fd = open(nome, O_WRONLY|O_CREAT|O_TRUNC, 0666);
  if(fd<0) {perror("Errore apertura file"); exit(1);}	
	
	//crea il vettore da scrivere su file in un colpo
	int vet[n];
	for(int i=1;i<=n;i++) {
		vet[i-1]= i;
	}
	// scrive valori sul file		 
	int e = write(fd,vet, sizeof(int)*n);   // scrive su f il contenuto di i 
	  if(e!=sizeof(int)*n) {
		perror("Errore scrittura file");
		exit(1);
	  }
	// chiude file 
	 e = close(fd);
	if(e!=0) {
	  perror("Errore chiusura file");
	  exit(1);
	}
}


int main(int argc, char *argv[])
{
  if(argc!=3) {
	  fprintf(stderr, "Uso:\n\t%s n nomefile\n",argv[0]);
	  exit(1);
  }
  int n = atoi(argv[1]);
  assert(n>0);
  scriviNn(n,argv[2]);
} 
	  
	  
	  
	  
