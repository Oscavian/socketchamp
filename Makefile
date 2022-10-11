CC = g++
CFLAGS = -g -std=c++17
SRCDIR = src
OBJDIR = obj

rebuild: clean all

all: webclient kekclient kekserver

clean:
	clear 
	rm -f bin/* obj/*


# VERY SIMPLE WEBCLIENT

obj/webclient.o: src/webclient.c
	$(CC) $(CFLAGS) -o obj/webclient.o src/webclient.c -c

webclient: obj/webclient.o
	$(CC) $(CFLAGS) -o bin/webclient obj/webclient.o


# CLIENT

obj/kekclient.o: src/kekclient.c
	$(CC) $(CFLAGS) -o obj/kekclient.o src/kekclient.c -c

kekclient: obj/kekclient.o
	$(CC) $(CFLAGS) -o bin/kekclient obj/kekclient.o


# SERVER

obj/kekserver.o: src/kekserver.c
	$(CC) $(CFLAGS) -o obj/kekserver.o src/kekclient.c -c

kekserver: obj/kekserver.o
	$(CC) $(CFLAGS) -o bin/kekserver obj/kekserver.o


