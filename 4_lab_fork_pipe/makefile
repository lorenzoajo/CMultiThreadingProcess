# makefile che utilizza regole implicite e variabili
#
# usare 
#    make <nome_eseguibile> 
# per compilare un unico programma, o semplicemente
#    make
# per compilarli tutti 
#
CFLAGS=-Wall -std=c99 -g -O -pthread
CC=gcc

# eseguibili da creare
EXECS=ordina_vet_pipe


all: $(EXECS)

ordina_vet_pipe: ordina_vet_pipe.o xerrors.o
	$(CC) $(CFLAGS) -o ordina_vet_pipe ordina_vet_pipe.o xerrors.o
	
ordina_vet_pipe.o: ordina_vet_pipe.c xerrors.h
	$(CC) $(CFLAGS) -c ordina_vet_pipe.c -o ordina_vet_pipe.o

clean:
	rm *.o $(EXECS)

