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
// I descrittori della pipe vengono usate con istruzioni write/read



// ************** Esercizio per casa ***********************************
// Modificare il programma in modo che i processi
// figli inviino (usando la write) al padre i numeri primi trovati
// Ogni valore deve essere inviato appena viene trovato, quindi i figli
// non devono memorizzare nulla.
// Il processo padre deve stampare su stdout i singoli valori ricevuti
// (i numeri primi) e al termine stampare su stderr il numero totale
// di valori ricevuti. Eseguendo il programma con la ridirezione di stdout
// i numeri primi possono essere salvati su un file a vostra scelta
// che dovrebbe avere un numero di linee pari al numeri di valori ricevuti
// scritto dal processo padre su stderr. Verificate quest'ultima cosa con 
// il comando wc



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
  int i,a,b,x,t,tot;
  pid_t pid;

  // crea un processo per ogni argomento sulla linea di comando
  for(i=0;i<p;i++) {
    if ((pid=fork())==0) {
      if(close(pipefd[0])!=0) perror("F chiusura pipe");
      a=1+(n/p)*i;
      b= (i+1==p) ? n : (n/p)*(i+1);
      fprintf(stderr,"Figlio %d cerca fra %d e %d\n",i,a,b);
      int tot = cerca(a,b);
      // scrivo i byte di tot
      n = write(pipefd[1],&tot,sizeof(tot));
      assert(n==sizeof(tot));
      exit(0);
    }
  }

  // Processo padre: attende terminazione dei figli e somma i valori 
  if(close(pipefd[1])!=0) perror("P chiusura pipe\n");
  tot = 0;
  do {
    t = read(pipefd[0],&x,sizeof(x)); // sizeof(x) restituisce il numero di byte usati dalla variabile x
    if(t!= sizeof(x)) break;
    tot += x;      
  } while(true); 
  fprintf(stderr, "Il numero di primi minori di %d e' %d\n",n,tot);
}



/* 
 * Volendo il ciclo do/while puo' essere sostituito dal seguente ciclo for
 * che aspetta la terminazione dei singoli processi
 * */
#if 0
  for(i=0;i<p;i++) { 
    pid=wait(NULL);
    if(pid== -1)
      perror("Errore nella wait");
    else{
      read(pipefd[0],&x,sizeof(x));
      tot +=x;
    }
  }
#endif
