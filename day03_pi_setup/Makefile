CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

all: test_rt

test_rt: test_rt.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f test_rt

.PHONY: all clean
