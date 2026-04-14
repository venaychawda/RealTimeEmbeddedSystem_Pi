# Day 7: POSIX Real-Time Clock & Jitter Measurement

## Goal
Measure wake-up jitter of a periodic RT service at 100 Hz.

## Build & Run
```bash
make
sudo ./jitter                  # no background load
stress --cpu 4 &               # add background load
sudo ./jitter                  # should show similar jitter (SCHED_FIFO preempts stress)
kill %1                        # stop stress
```

## Expected Output (no load)
```
Jitter over 100 iterations at 10 ms period:
  Avg: 15.3 us, Max: 85.2 us
```
Typical Pi 3B: avg 10–50 µs, max 50–200 µs. Under stress load, numbers should stay in the same ballpark.
