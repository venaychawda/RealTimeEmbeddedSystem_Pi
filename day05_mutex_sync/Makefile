CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

all: race

race: race.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f race

.PHONY: all clean
