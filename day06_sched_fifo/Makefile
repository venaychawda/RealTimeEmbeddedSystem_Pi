CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread -lrt

all: sched_demo

sched_demo: sched_demo.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f sched_demo

.PHONY: all clean
