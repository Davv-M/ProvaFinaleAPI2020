# Progetto di Algoritmi e Strutture Dati - AA 2019/20
Corso di laurea in Ingegneria Informatica, Politecnico di Milano

Docente: Prof. Matteo Pradella, A. A. 2019/20

Voto: 30 e lode/30

## Abstract
Lo scopo di questo progetto è la creazione di un editor di testo elementare, che supporta le operazioni di inserimento e modifica di righe di testo (change), di eliminazione
di righe precedentemente immesse (delete), di stampa a schermo di parte delle righe inserite (print) e di annullamento o ripristino di un numero arbitrario di comandi change e
delete (undo e redo). All'interno di Tema2020.pdf è possibile trovare altri dettagli relativi all'implementazione del progetto.

## Descrizione suite di test
Il programma è stato valutato tramite sottoposizione a sei diverse batterie di test, ciascuna delle quali è a sua volta composta da due task pubblici (l'input e il conseguente
output sono noti) e da tre task privati, basandosi sul tempo di esecuzione e sulla memoria occupata durante l'esecuzione. 
Oltre a queste, il programma è stato verificato con un task privato per l'assegnamento della lode. I dettagli di ciascuna suite di test sono indicati nella seguente tabella:

Test | Limite di tempo | Limite di memoria | Note
---- | --------- | --------- | ----
WriteOnly | 7,40 s | 3,00 GiB | Solo change e print, senza sovrascrittura delle righe 
BulkReads | 1,25 s | 240 MiB | Solo change e print, con possibile sovrascrittura delle righe
TimeForAChange | 2,10 s | 540 MiB | Solo change, print e delete
RollingBack | 2,10 s | 550 MiB | Tutti i tipi di comando, le undo e redo sono eseguite dopo una serie di change print e delete. Tra le operazioni di undo e redo si eseguono solo print
AlteringHistory | 2,10 s | 400 MiB | Tutti i tipi di comando, le undo e redo possono essere seguite da change o delete
Rollercoaster | 2,70 s | 1,03 GiB | Come AlteringHistory
Lode | 2,00 s | 340 MiB | Come AlteringHistory

## Risultati

### Memoria occupata

Test | Pubblico 1 (0 punti) | Pubblico 2 (0 punti) | Privato 1 (3 punti)| Privato 2 (1 punto) | Privato 3 (1 punto)
---- | --------- | --------- | ----- | ------ | ---- |
WriteOnly | 236 kiB | 236 kiB | 28,7 MiB | 38,2 MiB | 47,7 MiB
BulkReads | 236 kiB | 236 kiB | 68,8 MiB | 91,7 MiB | 115 MiB
TimeForAChange | 236 kiB | 236 kiB | 93,0 MiB | 142 MiB | 191 MiB
RollingBack | 3,98 MiB | 4,10 MiB | 102 MiB | 150 MiB | 199 MiB
AlteringHistory | 4,11 MiB | 4,10 MiB | 123 MiB | 181 MiB | 234 MiB
Rollercoaster | 4,10 MiB | 4,23 MiB | 299 MiB | 446 MiB | 593 MiB
Lode | - | - | - | 196 MiB | -

### Tempo di esecuzione

 Test | Pubblico 1 (0 punti) | Pubblico 2 (0 punti) | Privato 1 (3 punti)| Privato 2 (1 punto) | Privato 3 (1 punto)
---- | --------- | --------- | ----- | ------ | ----- |
WriteOnly | 0,001 s | 0,001 s | 0,129 s | 0,168 s | 0,213 s
BulkReads | 0,001 s | 0,001 s | 0,287 s | 0,387 s | 0,462 s
TimeForAChange | 0,002 s | 0,002 s | 0,355 s | 0,532 s | 0,699 s
RollingBack | 0,005 s | 0,008 s | 0,555 s | 1,021 s | 1,200 s
AlteringHistory | 0,007 s | 0,005 s | 0,927 s | 1,433 s | 1,293 s
Rollercoaster | 0,012 s | 0,010 s | 1,196 s | 1,811 s | 2,286 s
Lode | - | - | - | 0,876 s | -

## Compilazione ed esecuzione
Il codice è stato compilato tramite `gcc` usando i seguenti flag:
```
gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o main3 main3.c -lm
```
I vari casi di test sono invece sottoposti mediante semplice reindirizzamento di `stdin`:
```
./main3 < nomeInput.txt
```
