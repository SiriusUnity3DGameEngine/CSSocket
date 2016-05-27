all: server client

server: server.o common.o
	gcc -o server server.o common.o -lpthread -lrt -lm

server.o: server.c
	gcc -c server.c

client: client.o common.o
	gcc -o client client.o common.o -lpthread -lrt -lm

client.o: client.c
	gcc -c client.c

common.o: common.c
	gcc -c common.c

clean:
	rm -f *.o server client
