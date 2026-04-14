# Day 24: Multi-Core RMA & Core Isolation

## Goal
Isolate a CPU core for exclusive RT use and measure jitter improvement.

## Exercise
```bash
# Edit /boot/cmdline.txt — append: isolcpus=3
sudo nano /boot/cmdline.txt
sudo reboot

# After reboot, pin RT app to isolated core
sudo taskset -c 3 ./synchronome
```

## Expected Outcome
Max jitter drops 30–60% compared to non-isolated core.
