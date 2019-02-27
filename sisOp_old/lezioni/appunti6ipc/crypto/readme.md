=== Introduzione ===

Riferimenti sulla gestione delle password in Linux (files \etc\passwd e \etc\shadow) e sul worm che utilizzava un dizionario e la funzione crypt per violare le password: 

* https://en.wikipedia.org/wiki/Passwd

* https://en.wikipedia.org/wiki/Morris_worm


=== Programmi in questo archivio ===

encrypt
calcola l'hash di una password dato un salt e la password stessa

crypt_machine
cerca la password con tra le parole di un file dizionario e le loro varianti.
Il file dizionario deve avere una parola per riga. Sulla mia macchina ho usato
il file /usr/share/dict/words che contiene circa 100000 parole. La password cryptata da usare è quella restituita da encrypt

crypto_producer
Processo producer per la ricerca delle password da dizionario. Deve essere lanciato prima dei consumer con gli stessi parametri di crypt_machine

crypto_consumer
Processo consumer per la ricerca delle password. Possono essere lanciate diverse istanze di questo processo che collaboraranno alla ricerca della password. Gli deve essere passato come parametro la password cryptata. Attualmente tutti i processi che non trovano la password non terminano e devono essere terminati con killall

crypto_check
Visualizza lo stato delle variabili shared e dei semafori utilizzati da crypo_producer/consumer

crypto_killer
Dealloca tutte le memorie condivise e i semafori condivisi utilizzati da crypo_producer/consumer (attualmente rimangono allocati all'interno del sistema). 


=== Esercizi per casa ===

1. Capire come e perché funziona il makefile

2. Fare in modo che la password criptata venga salvata dal producer nella memoria condivisa in modo che i consumer non abbiano bisogno che essa venga passata sulla linea di comando

3. Aggiungere un meccanismo con il quale il producer possa segnalare ai consumer che non ci sono altre parole da analizzare. I consumer, ricevuto questo "segnale", devono terminare. 

4. Aggiungere un meccanismo con il quale il consumer che ha eventualmente trovato la password possa segnalare al producer che la ricerca è finita.



