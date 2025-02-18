CC = gcc
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g #-std=c11 -Wall -pedantic -fsanitize=address 

all: gettimings

gettimings: gettimings.o
	$(CC) $(CFLAGS) gettimings.o -o gettimings

gettimings.o: gettimings.c gettimings.h
	$(CC) $(CFLAGS) -c gettimings.c

clean:
	rm -f gettimings gettimings.o