.PHONY: clean dellcl dellserv

all: client server

client: client.c
	gcc -c client.c
	gcc client.o -o client
server: server.o func.o
	gcc -o server server.o func.o

server.o: server.c func.h
	gcc -c server.c

func.o: func.c func.h

dellcl:
	rm client.o client

dellserv:
	rm server.o server

clean:
	rm server.o server
	rm client.o client
	rm func.o
