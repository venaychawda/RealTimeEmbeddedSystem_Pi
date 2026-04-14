# Day 3: Pi Setup & Linux RT Environment

## Goal
Install all development tools and verify POSIX RT API availability.

## Setup
Run the top-level setup script (if not done already):
```bash
cd .. && ./scripts/setup_pi.sh
```

## Build & Run
```bash
make
sudo ./test_rt
```

## Expected Output
```
SCHED_FIFO priority range: 1 - 99
  ✓ POSIX RT scheduling is available. You have 99 RT priority levels.
```
If you see `-1 - -1`, your kernel does not support POSIX RT scheduling.

## Also Verify
```bash
lsusb                     # should list your USB camera
v4l2-ctl --list-devices   # should show /dev/video0
fswebcam -r 640x480 test.jpg  # snap a test photo
```
