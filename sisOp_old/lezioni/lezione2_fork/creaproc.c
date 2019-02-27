#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{ 
  pid_t n;
  if((n=fork())== (pid_t)-1)
  {perror("fork fallita");
   exit(1);
  }

  else if (n==(pid_t)0)
    {/* processo figlio */
     printf("Il risultato di fork() nel figlio e' %d\n",n);
     printf("Il pid del figlio e' %d\n",getpid());
     printf("Il pid del padre visto dal figlio e' %d\n",getppid());
    }
       else
    {/* processo padre */
     printf("Il risultato di fork() nel padre e' %d\n",n);
     printf("Il pid del padre e' %d\n",getpid());
     printf("Il pid del padre del padre e' %d\n",getppid());
    }
}
