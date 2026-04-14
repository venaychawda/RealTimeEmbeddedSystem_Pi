CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

all: prodcon

prodcon: producer_consumer.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f prodcon

.PHONY: all clean
