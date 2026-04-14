# Day 8: Cyclic Executive

## Goal
Build the simplest RT scheduler: a timed loop calling services in a fixed order.

## Build & Run
```bash
make
sudo ./cyclic_exec
```

## Expected Output (first ~10 lines)
```
[S1] Sensor read   @ 1830.123456
[S2] Processing    @ 1830.123478
[S3] Logging       @ 1830.123490
[S1] Sensor read   @ 1830.133502
[S1] Sensor read   @ 1830.143510
[S2] Processing    @ 1830.143525
[S3] Logging       @ 1830.143535
```

**Verify:** S1 every frame (~10ms apart), S2 every 2nd, S3 every 3rd. Frame 0 and 6 have all three.

## Challenge
Add a 4th service every 5th frame. Then add `usleep(15000)` inside S1 to simulate a 15ms overrun in a 10ms slot — observe the cascade delay.
