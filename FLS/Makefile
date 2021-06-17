CC=gcc
CFLAGS =-pthread -Wall -g
DEBUG 		= -DDEBUG
INCLUDE = linked_list.c
SOCKETNAME =fls

all:server client

d_all:sdebug cdebug

totalclean: clean Dirclean

server: server.c
	$(CC) $(CFLAGS) server.c  -o $@

client: client.c
	$(CC) $(CFLAGS) client.c  -o $@

sdebug: server.c
	$(CC) $(CFLAGS) include/List/linked_list.c include/chained_hash.c server.c  $(DEBUG) -lm -o server

cdebug: client.c
	$(CC) $(CFLAGS) include/List/linked_list.c include/chained_hash.c client.c  $(DEBUG) -o client

socketclean:
	unlink $(SOCKETNAME)

Dirclean: 
	rm -r CLIENT; mkdir CLIENT; rm -r SERVER; mkdir SERVER;

clean:
	rm server; rm client; unlink $(SOCKETNAME);

