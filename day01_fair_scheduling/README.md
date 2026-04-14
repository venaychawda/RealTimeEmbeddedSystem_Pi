# Day 1: What Are Real-Time Systems?

## Goal
Observe Linux's default fair scheduler (CFS) and understand why it is unsuitable for real-time workloads.

## Prerequisites
```bash
sudo apt install -y stress htop
```

## Exercise

Run the demo script which launches CPU stress on all 4 cores, then observe in htop that every process gets equal CPU time — no priority, no deadlines.

```bash
./run_demo.sh
```

## Expected Output

After running `stress --cpu 4 &`, open `htop` in another terminal. You should see:

- All 4 CPU bars at ~100%
- 4 `stress` processes each at ~25% total CPU
- No process is favoured over any other

This is **fair scheduling** — the OS distributes CPU equally. A pacemaker, ABS brake, or flight controller cannot tolerate this: a life-critical task could be starved by a background update.

## Reflection
Write a 1-paragraph answer: *Why would fair scheduling be dangerous in a pacemaker?*
