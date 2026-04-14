# Day 6: Linux Scheduling Policies — SCHED_FIFO

## Goal
See priority-preemptive scheduling in action: highest priority thread runs first.

## Build & Run
```bash
make
sudo taskset -c 0 ./sched_demo    # single core — priority ordering visible
sudo ./sched_demo                  # multi-core — threads may run in parallel
```

## Expected Output (single core: `taskset -c 0`)
```
Thread 2 (prio 90): 0.4650 sec, started at 1826.1150, ended at 1826.5800
Thread 1 (prio 50): 0.4640 sec, started at 1826.5801, ended at 1827.0441
Thread 0 (prio 10): 0.4630 sec, started at 1827.0442, ended at 1827.5072
```

**Verify:**
- Thread 2 (priority 90) starts and finishes **first**
- Thread 1 starts only after Thread 2 ends
- Thread 0 starts only after Thread 1 ends
- Total time ≈ 3× single-thread time (serial execution)

## Why the Semaphore Gate Matters
Without it, each thread preempts `main()` upon creation and runs to completion before the next is even born — giving the wrong impression that Thread 0 finishes first.
