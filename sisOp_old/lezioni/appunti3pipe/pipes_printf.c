#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h> 
#include <wait.h>
#include <sys/types.h>


// esempio di passaggio di informazione tra processi attraverso una pipe
// le pipe create vengono assegnate a stdout e stdin e quindi la
// lettura e scrittura avviene usando printf/scanf

int main(int argc, char *argv[])
{
  int pipefd[2];
  assert(argc==3);

  // crea una pipe da pipefd[1] a pipefd[0] 
  if(pipe(pipefd)!=0) {
    perror("Pipe fallita");
    exit(1);
  }
  pid_t n = fork();
  if(n==-1) {
     perror("Fork fallita");
     exit(1);
  }
  if (n==0) {
    /* figlio esegue il prodotto e lo scrive su pipfd[1] */
    dup2(pipefd[1],1);    // stdout diventa pipefd[1] permette di usare printf
    close(pipefd[0]);     // opportuno chiudere i descrittori non usati
    close(pipefd[1]); 
    printf("%d ",atoi(argv[1])*atoi(argv[2]));
    // nota: se non eseguo fflush i dati possono non essere inviate nella pipe a cuas del buffering
    // fflush(NULL); 
    // se non eseguo fflush e metto una sleep il padre non riceve il valore 
    // sleep(5);
    exit(0);
  }
  else {
    // padre legge il risultato da pipefd[0]
    dup2(pipefd[0],0);  // stdin diventa pipefd[0] permette di usare scanf
    close(pipefd[0]);   // opportuno chiudere i descrittori non usati
    close(pipefd[1]);
    int r;
    // Nota: non e' necessario attendere con wait
    // lascanf si blocca fino a quando non arriva la fine del numero o del file
    int v = scanf("%d",&r);
    if(v==EOF) 
      puts("EOF encountered");
    else
      fprintf(stderr, "Il risultato e' %d\n",r);  // uso stderr per non creare confusione con stdout 
  }
  return 0;
}
