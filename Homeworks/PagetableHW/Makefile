CC = gcc
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic -fsanitize=address 
LDFLAGS = -L. -lmlpt

all: libmlpt.a ptb

libmlpt.a: ptb.o
	ar rcs libmlpt.a ptb.o 

ptb: ptb.o
	$(CC) $(CFLAGS) ptb.o -o ptb

ptb.o: ptb.c mlpt.h config.h
	$(CC) $(CFLAGS) -c ptb.c

clean:
	rm -f ptb ptb.o libmlpt.a

.PHONY: all clean
