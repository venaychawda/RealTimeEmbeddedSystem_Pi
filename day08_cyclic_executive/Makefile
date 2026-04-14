CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lrt

all: cyclic_exec

cyclic_exec: cyclic_exec.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f cyclic_exec

.PHONY: all clean
