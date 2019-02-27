#include <stdio.h>    // permette di usare scanf printf .. 
#include <stdlib.h>   // conversioni stringa/numero rand() abs() exit() etc ...
#include <stdbool.h>  // gestisce tipo bool (per variabili booleane)
#include <assert.h>   // permette di usare la funzione assert


// leggi numero da file e restituisci la somma 
int leggi(char *nome) 
{
	// apre file in scrittura
	// FILE * f = xfopen(nome,"rb",__FILE__,__LINE__);
	FILE *f = fopen(nome,"rb");
	if(f==NULL) {  // se il file non è stato aperto visualizza messaggio d'errore e esci
	  perror("Errore apertura file");
	  exit(1);
	}
	// legge valori dal file
	int somma = 0;
	while(true) {
		int i;
		int e = fread(&i, sizeof(int), 1, f); // legge un int da f
		if(e==0) break; 
		if(e!=1) {
			perror("Errore lettura file");
			exit(1);
		}
		somma += i;
	}
	// chiude file 
	int e = fclose(f);
	if(e!=0) {
	  perror("Errore chiusura file");
	  exit(1);
	}
	return somma;
}


int main(int argc, char *argv[])
{
  if(argc!=2) {
	  fprintf(stderr, "Uso:\n\t%s nomefile\n",argv[0]);
	  exit(1);
  }
  int n = leggi(argv[1]);
  printf("Somma valori trovati: %d\n",n);
  return 0;
} 
