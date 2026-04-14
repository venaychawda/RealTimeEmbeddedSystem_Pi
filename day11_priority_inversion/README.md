# Day 11: Priority Inversion

## Goal
Observe priority inversion and learn the PTHREAD_PRIO_INHERIT fix.

## Build & Run
```bash
make
sudo taskset -c 0 ./prio_inv
```

## Expected Output
```
LOW: locking mutex
LOW: got mutex, working 200ms...
HIGH: trying to lock mutex...
LOW: releasing mutex
HIGH: got mutex after 150 ms (blocked!)
```

HIGH (prio 90) was blocked ~150ms by LOW (prio 10). Uncomment `PTHREAD_PRIO_INHERIT`, rebuild, and re-run to enable priority inheritance.
