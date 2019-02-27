#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

int down(int semid, int semnum) 
  /* operazione DOWN sulla componente semnum di semid */
{
  int r;
  struct sembuf sb;

  sb.sem_num=semnum;
  sb.sem_op=-1;
  sb.sem_flg=0;
  r=semop(semid,&sb,1);
  if (r==-1) perror("semop in down");
  return r;
}

int up(int semid, int semnum) 
   /* operazione UP sulla componente semnum di semid */
{
  int r;
  struct sembuf sb;

  sb.sem_num=semnum;
  sb.sem_op=1;
  sb.sem_flg=0;
  r=semop(semid,&sb,1);
  if (r==-1) perror("semop in up");
  return r;
}

int get_sem_val(int semid, int semnum)
  /* restituisce valore corrente componente semnum di semid */
{
  int r;
  r=semctl(semid,semnum,GETVAL,NULL);
  if (r==-1) perror("semop in getval");
  return r;
}


int seminit(int semid, int semnum, int initval)
  /* inizializzazione con initval della componente semnum di semid */
{
  int r;
  union semun
  {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  } arg;
  arg.val=initval;
  r=semctl(semid,semnum,SETVAL,arg);
  if (r==-1) perror("semop in init");
  return r;
}

