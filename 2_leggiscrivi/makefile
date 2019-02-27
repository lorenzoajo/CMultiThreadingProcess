# definizione variabili
CFLAGS=-Wall -std=c99 -g -O
CC=gcc

#eseguibili
EXECS=scrivi leggi leggisc scrivisc

all: $(EXECS)


scrivi: scrivi.o
	$(CC) $(CFLAGS) -o scrivi scrivi.o

leggi: leggi.o
	$(CC) $(CFLAGS) -o leggi leggi.o

leggisc: leggisc.o
	$(CC) $(CFLAGS) -o leggisc leggisc.o

scrivisc: scrivisc.o
	$(CC) $(CFLAGS) -o scrivisc scrivisc.o


clean:
	rm *.o $(EXECS)
