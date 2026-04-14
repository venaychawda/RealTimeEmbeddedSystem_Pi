# Day 5: POSIX Mutexes & Synchronization

## Goal
Observe a race condition, then fix it with a mutex.

## Build & Run
```bash
make
./race           # run several times — wrong answer each time
# Then uncomment the 3 mutex lines in race.c, recompile:
make clean && make
./race           # now always prints 2000000
```

## Expected Output
**Without mutex (broken):** `Counter = 1340287 (expected 2000000)` — number varies.
**With mutex (fixed):** `Counter = 2000000 (expected 2000000)` — every time.
