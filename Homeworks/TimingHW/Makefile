CC = gcc
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g #-std=c11 -Wall -pedantic -fsanitize=address 

all: gettimings

gettimings: main.o gettimings.o
	$(CC) $(CFLAGS) main.o gettimings.o -o gettimings

main.o: main.c gettimings.h
	$(CC) $(CFLAGS) -c main.c

gettimings.o: gettimings.c gettimings.h
	$(CC) $(CFLAGS) -c gettimings.c

clean:
	rm -f gettimings main.o gettimings.o