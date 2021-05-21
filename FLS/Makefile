CC=gcc
CFLAGS =-lpthread -Wall -g
DEBUG 		= -DDEBUG

server: server.c
	$(CC) server.c $(CFLAGS) -o $@

debug: server.c
	$(CC) server.c $(CFLAGS) $(DEBUG) -o server

clean:
	rm server
