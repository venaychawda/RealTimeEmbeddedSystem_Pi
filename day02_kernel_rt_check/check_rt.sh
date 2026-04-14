#!/bin/bash
# Day 2: Check kernel RT capabilities
echo "=== Kernel Version ==="
uname -a
echo ""

echo "=== Preemption Config ==="
if [ -f /boot/config-$(uname -r) ]; then
    grep PREEMPT /boot/config-$(uname -r) || echo "(not found in /boot/config)"
elif [ -f /proc/config.gz ]; then
    zcat /proc/config.gz | grep PREEMPT || echo "(not found in /proc/config.gz)"
else
    echo "Trying to load configs module..."
    sudo modprobe configs 2>/dev/null
    if [ -f /proc/config.gz ]; then
        zcat /proc/config.gz | grep PREEMPT
    else
        echo "Could not find kernel config. Day 3 test_rt will confirm RT support."
    fi
fi
echo ""

echo "=== POSIX RT Config ==="
if [ -f /proc/config.gz ]; then
    zcat /proc/config.gz | grep POSIX || echo "(not found)"
elif [ -f /boot/config-$(uname -r) ]; then
    grep POSIX /boot/config-$(uname -r) || echo "(not found)"
else
    echo "Kernel config not available. Run Day 3 test_rt to verify."
fi
echo ""
echo "=== Done ==="
