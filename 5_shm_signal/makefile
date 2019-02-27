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
EXECS=segnali shmsort shmerge


all: $(EXECS)

shmsort: shmsort.o xerrors.o
	$(CC) $(CFLAGS) -o shmsort shmsort.o xerrors.o
	
shmsort.o: shmsort.c xerrors.h
	$(CC) $(CFLAGS) -c shmsort.c -o shmsort.o

segnali: segnali.c 
	$(CC) $(CFLAGS) segnali.c -o segnali

shmerge: shmerge.o xerrors.o
	$(CC) $(CFLAGS) -o shmerge shmerge.o xerrors.o
	
shmerge.o: shmerge.c xerrors.h
	$(CC) $(CFLAGS) -c shmerge.c -o shmerge.o


clean:
	rm *.o $(EXECS)

