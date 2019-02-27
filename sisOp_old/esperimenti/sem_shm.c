#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>


//vogliamo creare un programma che da 0 fino a N controlla i nemeri primi
//con M processi figli e li scrive su un file primi.txt
//usa un buffer di dimensione M
// chamare con::::::::::::::: ./nome_eseguibile <numero primi da controllare> <numero processi figli>

typedef struct{
    int in;
    int out;
    int *vet;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
}struct_buff;

//prototipi
bool primo(int n);
void fun_figlio(int start,int end,int M, struct_buff *buff);

int main(int argc, char **argv){

    if(argc != 3){
        perror("ERRORE: numero di argomenti errato\n");
        exit(1);
    }

    int N = atoi(argv[1]); //numero a cui arrivare
    int M = atoi(argv[2]); //numero di processi figli

    //creazione memoria condivisa
    int shmid = shmget(IPC_PRIVATE,sizeof(struct_buff),0600);
    if(shmid == -1){
        perror("ERRORE: shmget\n");
        exit(2);
    }
    //presa della memoria condivisa
    struct_buff *buff = (struct_buff* ) shmat(shmid,0,0600);

    //inizializzazione delle variabili condivise
    buff->in=0;
    buff->out=0;
    buff->vet= malloc(sizeof(int)*M);
    if(buff->vet == NULL){
        perror("impossibile allocare memoria malloc vet\n");
        exit(2);
    }
    if(sem_init(&buff->empty,1,M) == -1)
        exit(2);
    if(sem_init(&buff->full,1,0) == -1)
        exit(2);
    pthread_mutex_unlock(&buff->mutex);


    //creazione processi figli
    int i = 0;
    for(i=0; i<M; i++){
        int pid_figlio = fork();

        if(pid_figlio == 0){
            //codice figli
            fun_figlio((N/M)*i,(N/M)*(i+1)-1,M,buff);

            printf("processo figlio uscito\n");

            exit(5);
        }
        else{
            //controllo su fork
            if(pid_figlio == -1){
                perror("ERRORE fork\n");
                return 0;
            }
        }
    }

    //codice padre
    FILE * fout = fopen("output_primi.txt","w");
    if(fout == NULL){
        perror("impossibile aprire file output\n");
        exit(6);
    }

    int cont = 0;
    while(cont <12){
        sem_wait(&buff->full);
        pthread_mutex_lock(&buff->mutex);
        printf("arrivo a scrivere qualcosa con il padre\n");
            fprintf(fout,"%d",buff->vet[buff->out]);
            buff->out = (buff->out++)%M;
        sem_post(&buff->empty);
        pthread_mutex_unlock(&buff->mutex);
    cont++;
    }

    fflush(fout);
    //chiusura e attesa figli
    for(i=0;i<M;i++){
    wait(NULL);
    }
		shmctl(shmid,IPC_RMID,NULL);
    return 0;
}

void fun_figlio(int start,int end,int M, struct_buff *buff){
    while(start<end){

        if(primo(start)){
            //entra in sezione critica solo se deve scrivere sul buffer
            sem_wait(&buff->empty);
            pthread_mutex_lock(&buff->mutex);
                buff->vet[buff->in]=start;
                buff->in = (buff->in ++) % M;
            sem_post(&buff->full);
            pthread_mutex_unlock(&buff->mutex);

        }

    start++;
    }

}



bool primo(int n)
{
  int i;
  if(n<2) return false;
  if(n%2==0) {
    if(n==2)
      return true;
    else
      return false;
  }
  for (i=3; i*i<=n; i += 2) {
      if(n%i==0) {
          return false;
      }
  }
  return true;
}
