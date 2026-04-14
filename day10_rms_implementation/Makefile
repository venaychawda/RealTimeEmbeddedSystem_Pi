CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread -lrt

all: rms_demo

rms_demo: rms_demo.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f rms_demo

.PHONY: all clean
