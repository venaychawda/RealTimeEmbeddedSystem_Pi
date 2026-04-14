# Day 14: POSIX Timers

## Build & Run
```bash
make
sudo ./posix_timer
```

## Expected Output (last few lines)
```
Tick 98 @ 31.103502
Tick 99 @ 31.113510
Total ticks: 100
```
~100 ticks at ~10ms intervals. If significantly fewer, signals may coalesce under load.
