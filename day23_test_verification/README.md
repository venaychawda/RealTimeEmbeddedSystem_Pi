# Day 23: Test & Verification Methods

## Goal
Stress-test your RT system and use kernel tracing to verify scheduling.

## Exercise
```bash
sudo apt install trace-cmd

# Record scheduling events
sudo trace-cmd record -e sched_switch -e sched_wakeup &
sudo ./sequencer
sudo trace-cmd stop
trace-cmd report | head -50

# Stress test
stress --cpu 4 --io 2 &
sudo ./synchronome
kill %1
```

## Expected Outcome
Trace shows your service threads switching in/out at expected intervals. Jitter under stress should be < 2× no-load jitter.
