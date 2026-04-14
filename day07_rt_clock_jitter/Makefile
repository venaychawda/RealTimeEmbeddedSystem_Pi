CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lrt

all: jitter

jitter: jitter.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f jitter

.PHONY: all clean
