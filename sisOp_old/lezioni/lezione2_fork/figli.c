#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// crea un processo per ogni argomento sulla linea di comando
// i processi creati si limitano ad aspettare per un numero di secondi
// pari al valore indicato sulla linea di comando.
// Al termine ogni processo figli restituisce al padre il proprio
// indice utilizzando la exit()
// Esempio:
//   figli 3 10 4
// crea 3 processi che aspettano rispettivamente per 3 10 e 4 secondi

// tiene occupato il processo per circa s secondi
void mysleep(int s) {
  while(s>0) {
    int j=0;
    while(j>=0) j++;
    s--;
  }
}

int main(int argc, char *argv[])
{
  int i,status;
  pid_t pid;

  // crea un processo per ogni argomento sulla linea di comando
  for(i=1;i<argc;i++)
    if ((pid=fork())==0) {
      int sec = atoi(argv[i]);
      mysleep(sec); 
      printf("F Figlio %d ha atteso per %d secs\n",i,sec);
      exit(i);
    }
    else  printf("P Figlio %d ha pid %d\n",i,pid);

  // attende terminazione dei figli 
  for(i=1;i<argc;i++) { 
    pid=wait(&status);
    // il valore della exit e' nei bit 8-15 di status
    if(pid!= -1)
      printf("P Terminato processo %d con exit %d\n",pid,(status>>8)&255);
    else perror("Errore nella wait");  
  }
}
