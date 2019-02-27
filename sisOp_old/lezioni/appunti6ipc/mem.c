/* 
   stampando il valore di alcuni puntatori, 
   illustra come vengono assegnati gli indirizzi (virtuali) 
   dal compilatore, e quali vengono restituiti dalla malloc e dalla shmat 

   il risultato su una particolare macchina e' 
   illustrato graficamente in mem.jpg
*/

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int ext1,ext2;

void f(int n)
{
  int a;

  static int s;
  
  /* stampa l'indirizzo del parametro, della variabile "automatica"
     e della variabile "statica" */

  printf("%p %p %p\n",&n, &a, &s);
  if (n>0) f(n-1);
}

int main()

{

int b,c,n;
char *p;

int shmid1,*addr1;
int shmid2;
char *addr2;

/* stampa l'indirizzo delle variabili globali e di quelle interne al main */

printf("var esterne: %p %p\n",&ext1,&ext2);

printf("interne al main: %p %p\n",&b, &c);

printf("dati della funzione: parametro, var. automatica, var. statica \n");

/* la funzione e' ricorsiva, f(5) da' luogo a 5 chiamate ricorsive:
   per ogni chiamata, sulla stack c'e' una copia diversa del parametro 
   e delle variabili locali - ma non di quelle statiche */

f(5);

/* stampa i valori restituiti da chiamate successive di malloc */

printf("malloc : %p\n",malloc(1024));
printf("malloc : %p\n",malloc(1024));
printf("malloc : %p\n",malloc(1024));
printf("malloc : %p\n",malloc(1024));

/* alloca memoria condivisa per un array di 100 interi e uno di 100 caratteri */

shmid1 = shmget(IPC_PRIVATE,100*sizeof(int),0600);
if (shmid1 == -1) perror("Creazione memoria condivisa");

shmid2 = shmget(IPC_PRIVATE,100*sizeof(char),0600);
if (shmid2 == -1) perror("Creazione memoria condivisa");

n= fork();
if (n == -1) perror("Fork");

if (n == 0)
   {  /* processo figlio */
        addr1 = (int *) shmat(shmid1, NULL, 0);
        addr2 = (char *) shmat(shmid2, NULL, 0);
	printf("shmat processo figlio al segmento di 100 interi: %p\n",addr1);
	printf("shmat processo figlio al segmento di 100 char: %p\n",addr2);
   }
else
   {  /* processo padre */
        addr2 = (char *) shmat(shmid2, 0, 0);
        addr1 = (int *) shmat(shmid1, 0, 0);
	printf("shmat processo padre al segmento di 100 interi: %p\n",addr1);
	printf("shmat processo padre al segmento di 100 char: %p\n",addr2);
	wait(NULL);
		shmctl(shmid1,IPC_RMID,NULL);
	shmctl(shmid2,IPC_RMID,NULL);
   }
}
