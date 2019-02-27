#define _GNU_SOURCE
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

bool test_variants(char *s, char *encrypted);
bool replace_and_test(char *s, int i, char c, char *encrypted);
bool test_single(char *s, char *encrypted);

int main(int argc, char *argv[])
{

  if(argc!=3) {
    fprintf(stderr,"USAGE:\n\t  %s $id$salt$hash  dictionary\n\n",argv[0]);
    fprintf(stderr,"Cerca una password tra le parole del file dictionary e le loro varianti\n");
    fprintf(stderr,"La password cryptata deve essere la stringa prodotta da encrypt (ricordare\n");
    fprintf(stderr,"che i caratteri $ sulla linea di comando devono essere indicati con \\$\n\n");
    return 1;
  }
  char s[100];
  FILE *f = fopen(argv[2],"r");
  if(f==NULL) {perror(argv[2]); return 2;}
  while(fgets(s,100,f)!=NULL) {
    // delete newline at end of s
    assert(s[strlen(s)-1]=='\n');
    s[strlen(s)-1]=0;
    // try password
    if(test_variants(s,argv[1])) {
      printf("Password found: %s\n",s); return 0;
    }
  }
  puts("No password in dictionary");
  return 4;
}

// verifica se s o una sua variante e' la password 
bool test_variants(char *s, char *encrypted) 
{
  if(test_single(s, encrypted)) return true;
  for(int i=0;i<strlen(s);i++) {
    // replace 1 for i
    if(s[i]=='i') 
      if(replace_and_test(s,i,'1',encrypted)) return true;
    // replace @ for a          
    if(s[i]=='a') 
      if(replace_and_test(s,i,'@',encrypted)) return true;
    // replace 0 for o          
    if(s[i]=='o') 
      if(replace_and_test(s,i,'0',encrypted)) return true;
  }
  return false;
}

// prova la password sostituendo s[i] con c 
bool replace_and_test(char *s, int i, char c, char *encrypted)
{
  char o = s[i];
  s[i] = c;
  if(test_single(s,encrypted)) return true;
  s[i] = o;
  return false;
}

// prova se s e' la password
bool test_single(char *s, char *encrypted) {
  char *c = crypt(s, encrypted);
  if(c==NULL) {puts("Error calling crypt"); exit(3);}
  return (strcmp(c,encrypted)==0);
}
