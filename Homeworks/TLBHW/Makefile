CC = gcc
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic -fsanitize=address 
LDFLAGS = -L. -ltlb

.PHONY: all clean

all: libtlb.a tlb

libtlb.a: tlb.o
	ar rcs libtlb.a tlb.o

tlb: tlb.o
	$(CC) $(CFLAGS) tlb.o -o tlb $(LDFLAGS)

tlb.o: tlb.c tlb.h mlpt.h config.h
	$(CC) $(CFLAGS) -c tlb.c

clean:
	rm -f tlb tlb.o libtlb.a 
