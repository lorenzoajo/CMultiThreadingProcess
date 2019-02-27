#include <stdio.h>    // permette di usare scanf printf .. 
#include <stdlib.h>   // conversioni stringa/numero rand() abs() exit() etc ...
#include <stdbool.h>  // gestisce tipo bool (per variabili booleane)
#include <assert.h>   // permette di usare la funzione assert


void scrivi1n(int n, char *nome) 
{
	// apre file in scrittura
	FILE *f = fopen(nome,"wb");
	if(f==NULL) {  // se il file non è stato aperto visualizza messaggio d'errore e esci
	  perror("Errore apertura file");
	  exit(1);
	}
	// scrive valori sul file
	for(int i=1;i<=n;i++) {
	  int e = fwrite(&i, sizeof(int), 1, f); // scrive su f il contenuto di i 
	  if(e!=1) {
		  perror("Errore scrittura file");
		  exit(1);
	  }
	}
	// chiude file 
	int e = fclose(f);
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
  scrivi1n(n,argv[2]);
} 
	  
	  
	  
	  
