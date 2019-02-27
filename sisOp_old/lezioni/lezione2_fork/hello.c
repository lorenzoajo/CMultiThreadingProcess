#include <stdio.h>

int main(int argc, char *argv[])
{
  if(argc==1)
    printf("Hello \n");
  else
    printf("Hello con argomento %s\n",argv[1]);

}
