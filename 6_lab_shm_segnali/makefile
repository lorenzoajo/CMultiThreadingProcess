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
EXECS=primiP


all: $(EXECS)

primiP: primiP.o xerrors.o
	$(CC) $(CFLAGS) -o primiP primiP.o xerrors.o
	
primiP.o: primiP.c xerrors.h
	$(CC) $(CFLAGS) -c primiP.c -o primiP.o

clean:
	rm *.o $(EXECS)

