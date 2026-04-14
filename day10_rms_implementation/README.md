# Day 10: Rate Monotonic Scheduling — Implementation

## Goal
Implement the 3-task RMA set from Day 9 and observe priority-driven execution.

## Build & Run
```bash
make
sudo taskset -c 0 ./rms_demo | head -20
```

## Expected Output
```
T1 start @ 30.100
T1 start @ 30.110
T2 start @ 30.112
T1 start @ 30.120
T1 start @ 30.130
T2 start @ 30.132
...
T3 start @ 30.156
```

**Verify:** T1 most frequent (highest priority), T2 after T1 completes, T3 every ~50ms.

## Challenge
Increase T3's `wcet_ms` from 10 → 15 → 20. Observe deadline misses. Verify against Day 9 LUB calculation.
