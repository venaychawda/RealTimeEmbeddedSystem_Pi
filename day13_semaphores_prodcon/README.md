# Day 13: POSIX Semaphores — Producer/Consumer

## Build & Run
```bash
make
./prodcon
```

## Expected Output
```
Produced: 0 at slot 0
Produced: 1 at slot 1
  Consumed: 0 from slot 0
Produced: 2 at slot 2
...
  Consumed: 19 from slot 4
```
Items consumed in FIFO order. Producer gets ahead, then blocks when buffer full (5 slots).
