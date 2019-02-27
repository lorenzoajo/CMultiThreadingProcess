# ---- costanti usate da tutti i programmi
# chiave usata per shmget e semget
key=256
# dimensione buffer producer/consumer
bufsize=10
# massima lunghezza password (incluso \n e \0)    
pwdsize=30

# flag usati per la compilazione di tutti gli eseguibili
CFLAGS=-Wall -O2 -std=c99 -DMYKEY=$(key) -DBUFSIZE=$(bufsize) -DPWDSIZE=$(pwdsize)
# definizione del compilatore 
CC=gcc
# librerie con cui linkare
LDFLAGS=-lcrypt

#------------- eseguibili da creare
tools=crypto_producer crypto_consumer crypto_check crypto_killer

# target di default del makefile (tutti gli eseguibili)
crypto: encrypt crypt_machine $(tools)

# encrypt e crypt_machine sono programmi stand-alone 
encrypt: encrypt.o
	gcc -o encrypt encrypt.o -lcrypt

crypt_machine: crypt_machine.o
	gcc -o crypt_machine crypt_machine.o -lcrypt


# producer e consumer dipendono da semfun.o
crypto_producer: crypto_producer.o semfun.o
	gcc -o crypto_producer crypto_producer.o semfun.o

crypto_consumer: crypto_consumer.o semfun.o
	gcc -o crypto_consumer crypto_consumer.o semfun.o -lcrypt

semfun.o: semfun.c semfun.h
	gcc -c semfun.c

# ----- eseguire questo se si cambiano le costanti
clean:
	rm *.o $(execs)
	
