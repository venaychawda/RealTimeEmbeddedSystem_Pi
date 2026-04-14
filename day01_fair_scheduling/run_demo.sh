#!/bin/bash
# Day 1: Observe fair scheduling under full CPU load
echo "=== Day 1: Fair Scheduling Demo ==="
echo ""
echo "Launching 4 CPU stress workers (one per core)..."
stress --cpu 4 &
STRESS_PID=$!
echo "Stress PID: $STRESS_PID"
echo ""
echo "Open another terminal and run: htop"
echo "Observe: all 4 CPU bars at ~100%, each worker gets equal time."
echo ""
echo "Press ENTER to stop stress and exit..."
read
kill $STRESS_PID 2>/dev/null
echo "Stress workers stopped."
