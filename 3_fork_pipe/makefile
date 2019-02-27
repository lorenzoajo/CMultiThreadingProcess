# makefile che utilizza regole implicite e variabili
#
# usare 
#    make <nome_eseguibile> 
# per compilare un unico programma, o semplicemente
#    make
# per compilarli tutti 
#
CFLAGS=-Wall -std=c99 -g -O
CC=gcc

# eseguibili da creare
EXECS=contaprimi contapar contaparpipe contaprimiexit


all: $(EXECS)

contaprimi: contaprimi.o primo.o
	$(CC) $(CFLAGS) -o contaprimi contaprimi.o primo.o

contapar: contapar.o primo.o xerrors.o
	$(CC) $(CFLAGS) -o contapar contapar.o primo.o xerrors.o

contaparpipe: contaparpipe.o primo.o  xerrors.o
	$(CC) $(CFLAGS) -o contaparpipe contaparpipe.o primo.o xerrors.o
	
contaprimiexit: contaparexit.o primo.o  xerrors.o
	$(CC) $(CFLAGS) -o contaprimiexit contaparexit.o primo.o xerrors.o

contaparexit.o: contaparexit.c xerrors.h
	$(CC) $(CFLAGS) -c contaparexit.c -o contaparexit.o

# regola per contapar.o che usa anche la dipendenza da xerrors.h
contapar.o: contapar.c xerrors.h
	$(CC) $(CFLAGS) -c contapar.c -o contapar.o

# idem per contaparpipe	 
contaparpipe.o: contaparpipe.c xerrors.h
	$(CC) $(CFLAGS) -c contaparpipe.c -o contaparpipe.o

clean:
	rm *.o $(EXECS)

