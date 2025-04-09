CC = gcc
CFLAGS = -g -Og -Wall
LDFLAGS = -L. -lnetsim

.PHONY: all clean

all: netlab

orig: netlab_orig.o
	$(CC) netlab_orig.o -o netlab_orig $(LDFLAGS) 

netlab: netlab.o
	$(CC) netlab.o -o netlab $(LDFLAGS) 

%.o : %.c netsim.h
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	rm -f *.o netlab netlab_orig
