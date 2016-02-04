CC = gcc -Wall -pthread

all: client server

clean:
	rm -f *.o core.* client server
    
client: client.o msg.o
	$(CC) -o client client.o msg.o
server: server.o msg.o
	$(CC) -o server server.o msg.o

client.o: client.c client.h
	$(CC) -c client.c 
server.o: server.c server.h
	$(CC) -c server.c 
msg.o: msg.c msg.h
	$(CC) -c msg.c 
    