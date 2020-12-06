#Makefile for EE450 Term Project Part 3
.PHONY: mainServer dbServer calcServer

all:
	gcc -g -Wall -o mainServer mainServer.c
	gcc -g -Wall -o dbServer dbServer.c
	gcc -g -Wall -o calcServer calcServer.c
	gcc -g -Wall -o client client.c

mainServer:
	./mainServer

dbServer:
	./dbServer

calcServer:
	./calcServer

clean:
	rm mainServer dbServer calcServer client