int down(int semid, int semnum);
	/* operazione DOWN sulla componente semnum di semid */

int up(int semid, int semnum);
	/* operazione UP sulla componente semnum di semid */

int get_sem_val(int semid, int semnum);
  /* restituisce valore corrente componente semnum di semid */

int seminit(int semid, int semnum, int initval);
	/* inizializzazione con initval della componente semnum di semid */
