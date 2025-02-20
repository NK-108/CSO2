CC = gcc-12
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic -fsanitize=address -ftrivial-auto-var-init=pattern

all: fork_run

fork_run: fork_run.o
	$(CC) $(CFLAGS) fork_run.o -o fork_run

fork_run.o: fork_run.c
	$(CC) $(CFLAGS) -c fork_run.c

clean:
	rm -f fork_run fork_run.o