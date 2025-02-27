CC = gcc
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic -fsanitize=address 

all: ptb

ptb: ptb.o
	$(CC) $(CFLAGS) ptb.o -o ptb

ptb.o: ptb.c config.h mlpt.h
	$(CC) $(CFLAGS) -c ptb.c

clean:
	rm -f ptb ptb.o