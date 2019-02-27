#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{ 
  pid_t n;
  if((n=fork())== (pid_t)-1) {
    perror("fork fallita");
    exit(1);
  }

  else if (n==(pid_t)0) { 
    /* processo figlio: esegue il comando hello */
      execl("hello","hello","ciao", NULL);
      // la exec ritorna solo se fallisce 
      perror("exec figlio fallita");
    }
    else {/* processo padre: esegue il comando "ls /home"*/
      execlp("ls","ls","/home", NULL);
      // la exec ritorna solo se fallisce 
      perror("exec padre fallita");
    }
}

