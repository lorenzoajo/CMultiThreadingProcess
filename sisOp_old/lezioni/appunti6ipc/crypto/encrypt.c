#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>


// compile with -lcrypt

// alternative command to generate the MD5 passwords:
// openssl passwd -1 -salt xyz  yourpass


int main(int argc, char *argv[])
{

  if(argc!=3) {
    fprintf(stderr,"USAGE:\n\t  %s salt  password\n\n",argv[0]);
    fprintf(stderr,"Calcola hash MD5 di una password per una data stringa di salt\n\n");
    return 1;
  }
  
  // crea stringa con algoritmo e salt  
  char *s;
  if(asprintf(&s,"$1$%s",argv[1])==-1) {
    puts("Error creating salt string");
    return 1;
  } 
  // encrypt password 
  char *c = crypt(argv[2],s);
  if(c==NULL) {
      puts("Error calling crypt"); return 2;
  }
  puts(c);
  free(s);
  return 0;
}
