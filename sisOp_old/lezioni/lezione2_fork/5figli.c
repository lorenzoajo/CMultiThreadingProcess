#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void proc(int i)
{ int n;
  printf("Processo %d con pid %d\n",i,getpid());
  for (n=0;n<500000000;n++);
}


int main()
{
int i;
pid_t pid;

for(i=0;i<5;i++)
  if (fork()==0)
    { proc(i); exit(0);};
for(i=0;i<5;i++)
  { pid=wait(NULL);
    printf("Terminato processo %d\n",pid);
  }
}
