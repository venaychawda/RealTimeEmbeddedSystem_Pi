CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lrt

all: posix_timer

posix_timer: posix_timer.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f posix_timer

.PHONY: all clean
