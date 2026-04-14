# Day 2: Real-Time Implementation Options Survey

## Goal
Verify that your Pi's kernel supports POSIX real-time features and preemption.

## Exercise

Run the check script to inspect your kernel configuration:

```bash
./check_rt.sh
```

## Expected Output

```
=== Kernel Version ===
Linux raspberrypi 6.1.21-v8+ #1642 SMP PREEMPT ... aarch64 GNU/Linux
  ✓ Look for: PREEMPT and SMP in the output

=== Preemption Config ===
CONFIG_PREEMPT=y
  ✓ Full preemption enabled

=== POSIX RT Config ===
CONFIG_POSIX_MQUEUE=y
CONFIG_POSIX_TIMERS=y
  ✓ POSIX real-time APIs available
```

If `CONFIG_PREEMPT=y` is missing, your kernel has weaker RT guarantees but the exercises will still work.

## Reading
- Wikipedia: "Real-time computing"
- FreeRTOS.org → "What is an RTOS?"
- POSIX.1b (IEEE 1003.1b) overview

## Written Exercise
Create a comparison table: Cyclic Executive vs. RTOS vs. Linux+POSIX RT
Columns: complexity, determinism, cost, multi-core support.
