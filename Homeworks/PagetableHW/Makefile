CC = gcc
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic -fsanitize=address 
LDFLAGS = -L. -lmlpt

.PHONY: all clean

all: libmlpt.a mlpt

libmlpt.a: mlpt.o
	ar rcs libmlpt.a mlpt.o 

mlpt: mlpt.o
	$(CC) $(CFLAGS) mlpt.o -o mlpt $(LDFLAGS)

mlpt.o: mlpt.c mlpt.h config.h
	$(CC) $(CFLAGS) -c mlpt.c

clean:
	rm -f mlpt mlpt.o libmlpt.a
