CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread -lrt

all: sequencer

sequencer: sequencer.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f sequencer

.PHONY: all clean
