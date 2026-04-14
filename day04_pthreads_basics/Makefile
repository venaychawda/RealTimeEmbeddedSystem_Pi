CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

all: pthread_hello

pthread_hello: pthread_hello.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f pthread_hello

.PHONY: all clean
