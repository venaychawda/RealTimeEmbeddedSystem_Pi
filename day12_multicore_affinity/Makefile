CC = gcc
CFLAGS = -Wall -g 
LDFLAGS = -lpthread -lrt

all: multicore_rt

multicore_rt: multicore_rt.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f multicore_rt

.PHONY: all clean
