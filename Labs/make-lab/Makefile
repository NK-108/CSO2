CC = gcc
CDFLAGS = -Wall -pedantic -std=c11 -fsanitize=address
LDFLAGS = -Wall -pedantic -fsanitize=address

.PHONY: all clean

all: guesser
	$(CC) -o guesser guesser.o -L. -lsay

clean:
	rm cheer.o
	rm grunt.o
	rm guesser.o
	rm libsay.a
	rm guesser

%.o: %.c say.h
	$(CC) -c $(CFLAGS) $< -o $@

libsay.a: cheer.o grunt.o
	ar -rcs libsay.a cheer.o grunt.o

guesser: libsay.a guesser.o
	$(CC) -o guesser guesser.o -L. -lsay


