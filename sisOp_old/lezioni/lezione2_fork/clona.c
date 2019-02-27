#include <stdio.h>
#include <unistd.h>

// prima di eseguire questo programma 
// provate a indovinare quanti messaggi "Hello World"
// saranno stampati

int main()
{
  fork();
  printf("Hello World from process %d\n",getpid());
  fork();
  printf("Hello World from process %d\n",getpid());
  fork();
  printf("Hello World from process %d\n",getpid());
}
