CC=gcc
CFLAGS =-lpthread -Wall -g
DEBUG 		= -DDEBUG
INCLUDE = linked_list.c

all:server client

server: server.c
	$(CC) $(CFLAGS) server.c  -o $@

client: client.c
	$(CC) $(CFLAGS) client.c  -o $@

sdebug: server.c
	$(CC) $(CFLAGS) server.c  $(DEBUG) -o server

cdebug: client.c
	$(CC) $(CFLAGS) client.c  $(DEBUG) -o client

clean:
	rm server; rm client;
