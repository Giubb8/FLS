CC=gcc
CFLAGS =-pthread -Wall -g
DEBUG 		= -DDEBUG
INCLUDE = linked_list.c
SOCKETNAME =fls

all:server client

d_all:sdebug cdebug

server: server.c
	$(CC) $(CFLAGS) server.c  -o $@

client: client.c
	$(CC) $(CFLAGS) client.c  -o $@

sdebug: server.c
	$(CC) $(CFLAGS) include/List/linked_list.c server.c  $(DEBUG) -o server

cdebug: client.c
	$(CC) $(CFLAGS) include/List/linked_list.c client.c  $(DEBUG) -o client

clean:
	rm server; rm client; unlink $(SOCKETNAME);
