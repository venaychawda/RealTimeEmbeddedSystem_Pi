# Day 4: POSIX Threads (pthreads) Basics

## Goal
Learn thread creation, joining, and observe non-deterministic interleaving.

## Build & Run
```bash
make
./pthread_hello
```

## Expected Output (order varies each run)
```
Thread 0: iteration 0
Thread 1: iteration 0
Thread 2: iteration 0
Thread 0: iteration 1
Thread 2: iteration 1
Thread 1: iteration 1
...
All threads done.
```

Run 3–4 times — the interleaving order changes each run. No `sudo` needed (default scheduling).
