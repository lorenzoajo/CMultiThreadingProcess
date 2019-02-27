#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  int i,j;
  pid_t n;

  n=fork();
  if (n==(pid_t)-1)
  {perror("fork fallita");
   exit(1);
  };
  if (n==(pid_t)0) {
      for (j=0;j<50;j++) {
        for (i=0; i< 100000000; i++);
        printf("     Figlio %d \n",j);
      }
  }
  else {
      for (j=0;j<50;j++) {
        for (i=0; i< 100000000; i++);
        printf("Padre %d\n",j);
      }
  }
}
