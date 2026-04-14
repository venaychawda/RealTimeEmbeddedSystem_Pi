# Day 12: Multi-Core CPU Affinity

## Build & Run
```bash
make
sudo ./multicore_rt
```

## Expected Output (4 cores)
```
Task 0 ran on CPU 0, took 50 ms
Task 1 ran on CPU 1, took 50 ms
Task 2 ran on CPU 2, took 50 ms
Task 3 ran on CPU 3, took 50 ms
```
Total wall-clock: ~50 ms (parallel). Change `CPU_SET(i,...)` to `CPU_SET(0,...)` — total becomes ~200 ms (serial).
