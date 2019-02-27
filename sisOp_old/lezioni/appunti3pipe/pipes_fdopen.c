#define _GNU_SOURCE   // richiesto sul mio sistema per evitare warning su fdopen
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>  // gestisce tipo bool (per variabili booleane)
#include <assert.h>   // permette di usare la funzione assert


// Programma che calcola il numero primi in un intervallo

// Il passaggio dei valori (# primi) tra figli e padre attraverso una pipe
// I descrittori della pipe vengono trasformati in FILE * attraverso fdopen

// Strategia NON sicura al 100% in quanto l'uso simultaneo 
// dello stesso file (in scrittura) da parte dei figli usando printf
// potrebbe causare il mischiarsi dei valori inviati al padre.



// restituisce true/false a seconda che n sia primo o composto
bool primo(int n)
{
  int i;
  if(n<2) return false;
  if(n%2==0) {
    if(n==2)
      return true;
    else
      return false;
  }
  for (i=3; i*i<=n; i += 2) {
      if(n%i==0) {
          return false;
      }
  }
  return true;
}

//conta quanti sono i primi tra a e b (escluso)
int cerca(int a, int b)
{
  int tot = 0;
  for(int i=a;i<b;i++)
    if(primo(i)) tot++;
  return tot;  
}


// Nota: i messaggi di errore e le informazioni di servizio
// vengono stampate su stderr per non interferire con stdout 
// La funzione perror e' gia' definita in modo da 
// mandare il suo output su stderr
int main(int argc, char *argv[])
{
  // leggi input da linea di comando
  if(argc!=3) {
    fprintf(stderr,"Uso\n\t%s limite num_processi\n", argv[0]);
    exit(1);
  }
  int n = atoi(argv[1]);
  int p = atoi(argv[2]);
  assert(n>0 && p>0);

  // crea una pipe
  int pipefd[2];
  if(pipe(pipefd)!=0) {
    perror("Pipe fallita");
    exit(1);
  }

  // possiamo procedere: crea processi
  int i,a,b,x,tot;
  pid_t pid;

  // crea un processo per ogni argomento sulla linea di comando
  for(i=0;i<p;i++) {
    if ((pid=fork())==0) {
      if(close(pipefd[0])!=0) perror("F chiusura pipe");
      a=1+(n/p)*i;
      b= (i+1==p) ? n : (n/p)*(i+1);
      fprintf(stderr,"Figlio %d cerca fra %d e %d\n",i,a,b);
      int tot = cerca(a,b);
      // creo file da file descriptor 
      FILE *f = fdopen(pipefd[1],"w");
      fprintf(f,"%d\n",tot);  // lo \n e' importante perche' indica la fine del valore (provate a torglierlo e a veder cosa succede)
      // se metto una sleep qui vedo che fino a quando non chiudo il file
      // o faccio fflush i dati non vengono inviati a causa del buffering.   
      fclose(f);
      exit(0);
    }
  }

  // Processo padre: attende terminazione dei figli e somma i valori 
  FILE *f = fdopen(pipefd[0],"r");   // associa pipfd[0] ad un FILE * 
  if(close(pipefd[1])!=0) perror("P chiusura pipe\n"); // chiude estremita della pipe non usata
  tot = 0;
  while( fscanf(f,"%d",&x)!=EOF ) 
    tot += x;
  fclose(f);
  fprintf(stderr, "Il numero di primi minori di %d e' %d\n",n,tot);
}


/* 
 * Volendo il ciclo while puo' essere sostituito dal seguente ciclo for
 * che aspetta la terminazione dei singoli processi
 * */
#if 0
  for(i=0;i<p;i++) { 
    pid=wait(NULL);
    if(pid== -1)
      perror("Errore nella wait");
    else{
      fscanf(f,"%d",&x);
      tot +=x;
    }
  }
#endif
