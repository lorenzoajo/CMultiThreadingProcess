#include <stdio.h>    // permette di usare scanf printf .. 
#include <stdlib.h>   // conversioni stringa/numero rand() abs() exit() etc ...
#include <stdbool.h>  // gestisce tipo bool (per variabili booleane)
#include <assert.h>   // permette di usare la funzione assert
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


// leggi numero da file e restituisci la somma 
int leggi(char *nome,int b) 
{
	// apre file in scrittura
	int fd = open(nome,O_RDONLY);
	if(fd<0) {perror("Errore apertura file"); exit(1);}
	// legge valori dal file
	int somma = 0;
	int i = 0;
	while(true) {
		int vet[b];
		int e = read(fd, vet, sizeof(int)*b); // legge un blocco di b int da f
		if(e==0) break; // fine del file
		if(e<sizeof(int)*b) {
			for(i=0;i<(e/sizeof(int));i++){
				somma += vet[i];
			}
			break;//fine del file con rimanenze inferiori a b interi
		}
		for(i=0;i<b;i++){
			somma += vet[i];
		}	
	}
	// chiude file 
	int e = close(fd);
	if(e<0) {
	  perror("Errore chiusura file");
	  exit(1);
	}
	return somma;
}


int main(int argc, char *argv[])
{
  if(argc!=3) {
	  fprintf(stderr, "Uso:\n\t%s b(dimensioni blocchi da leggere) nomefile\n",argv[0]);
	  exit(1);
  }
  int n = leggi(argv[2],atoi(argv[1]));
  printf("Somma valori trovati: %d\n",n);
  return 0;
}
