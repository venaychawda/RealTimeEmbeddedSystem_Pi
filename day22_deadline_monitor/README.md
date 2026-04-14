# Day 22: Deadline Monitoring & Overrun Handling

## Goal
Add runtime deadline checking to your sequencer.

## Code Snippet (add after each service's work)
```c
clock_gettime(CLOCK_MONOTONIC, &actual);
long overrun_us = ((actual.tv_sec - expected.tv_sec) * 1000000L +
                   (actual.tv_nsec - expected.tv_nsec) / 1000);
if (overrun_us > DEADLINE_US) {
    printf("DEADLINE MISS: S%d overran by %ld us\n", id, overrun_us);
    missed_deadlines++;
}
```

## Expected Output
Normal operation: no `DEADLINE MISS` lines. With artificial overload: `DEADLINE MISS: S1 overran by 5230 us`.
