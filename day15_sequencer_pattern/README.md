# Day 15: Sequencer Pattern

## Goal
Build the backbone RT architecture: a sequencer releasing multi-rate services.

## Build & Run
```bash
make
sudo taskset -c 0 ./sequencer | head -20
```

## Expected Output
```
S1 @ 45.100
S2 @ 45.100
S3 @ 45.100
S1 @ 45.110
S1 @ 45.120
S2 @ 45.120
S1 @ 45.130
S1 @ 45.140
S2 @ 45.140
S1 @ 45.150
S3 @ 45.150
```

Frame 0: all three fire. S1 every 10ms, S2 every 20ms, S3 every 50ms.
When multiple fire in the same frame, highest priority (S1) prints first.
