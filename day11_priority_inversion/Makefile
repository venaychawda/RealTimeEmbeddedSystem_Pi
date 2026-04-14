CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread -lrt

all: prio_inv

prio_inv: priority_inversion.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f prio_inv

.PHONY: all clean
