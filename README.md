# Real-Time Embedded Systems Training Lab

> A 25-day, hands-on training course for learning real-time embedded systems programming on a Raspberry Pi 3B with a USB camera, using Linux and POSIX real-time extensions.

---

## Mission

Bring university-grade real-time systems education to anyone with a Raspberry Pi and a USB camera. Every concept is paired with working code you compile, run, measure, and reason about — no simulators, no theory-only lectures.

By the end of this course you will be able to:

- Explain the difference between hard, soft, and best-effort real-time systems
- Use POSIX real-time scheduling (`SCHED_FIFO`) to guarantee deadline-oriented execution
- Perform Rate Monotonic Analysis (RMA) and verify schedulability by hand and empirically
- Diagnose and fix priority inversion using priority inheritance
- Build a multi-service real-time system using the sequencer pattern
- Capture and process camera frames under real-time constraints
- Measure jitter, worst-case execution time, and verify timing guarantees under load

## Hardware Requirements

| Item | Notes |
|------|-------|
| Raspberry Pi 3B | 4-core ARM Cortex-A53, running Raspbian OS (Bullseye/Bookworm Lite) |
| USB Webcam | Any UVC-compatible camera (e.g. Logitech C270) |
| SD Card | 16 GB minimum |
| Power Supply | Official 5V/2.5A recommended |
| Access | SSH, or monitor + keyboard |

## Software Prerequisites

- Comfort with C programming and basic Linux terminal commands
- Git (to clone this repo)

All other dependencies are installed as part of Day 3. A setup script is also provided:

```bash
./scripts/setup_pi.sh
```

## Repository Structure

```
rtes-training/
├── README.md                  ← You are here
├── TRAINING_PLAN.md           ← Full 25-day plan with theory, explanations, expected output
├── Makefile                   ← Top-level: build all days, or a single day
├── scripts/
│   └── setup_pi.sh            ← One-shot Pi environment setup
│
├── day01_fair_scheduling/     ← Week 1: Foundations
│   ├── README.md
│   └── run_demo.sh
├── day02_kernel_rt_check/
│   ├── README.md
│   └── check_rt.sh
├── day03_pi_setup/
│   ├── README.md
│   ├── test_rt.c
│   └── Makefile
├── day04_pthreads_basics/
│   ├── README.md
│   ├── pthread_hello.c
│   └── Makefile
├── day05_mutex_sync/
│   ├── README.md
│   ├── race.c
│   └── Makefile
│
├── day06_sched_fifo/          ← Week 2: RT Scheduling
│   ├── README.md
│   ├── sched_demo.c
│   └── Makefile
├── day07_rt_clock_jitter/
│   ├── README.md
│   ├── jitter.c
│   └── Makefile
├── day08_cyclic_executive/
│   ├── README.md
│   ├── cyclic_exec.c
│   └── Makefile
├── day09_rma_theory/
│   └── README.md              ← Hand-calculation exercises with answers
├── day10_rms_implementation/
│   ├── README.md
│   ├── rms_demo.c
│   └── Makefile
│
├── day11_priority_inversion/  ← Week 3: Interference, Blocking, Multi-Core
│   ├── README.md
│   ├── priority_inversion.c
│   └── Makefile
├── day12_multicore_affinity/
│   ├── README.md
│   ├── multicore_rt.c
│   └── Makefile
├── day13_semaphores_prodcon/
│   ├── README.md
│   ├── producer_consumer.c
│   └── Makefile
├── day14_posix_timers/
│   ├── README.md
│   ├── posix_timer.c
│   └── Makefile
├── day15_sequencer_pattern/
│   ├── README.md
│   ├── sequencer.c
│   └── Makefile
│
├── day16_v4l2_camera/         ← Week 4: Camera & Vision
│   ├── README.md
│   └── capture_frames.sh
├── day17_opencv_capture/
│   ├── README.md
│   ├── cam_capture.c
│   └── Makefile
├── day18_rt_frame_capture/
│   └── README.md              ← Integration exercise (self-directed)
├── day19_frame_differencing/
│   ├── README.md
│   └── motion.py
├── day20_wcet_analysis/
│   └── README.md              ← Measurement exercise
│
├── day21_synchronome/         ← Week 5: Integration & Verification
│   └── README.md
├── day22_deadline_monitor/
│   └── README.md
├── day23_test_verification/
│   └── README.md
├── day24_multicore_rma/
│   └── README.md
└── day25_capstone/
    └── README.md              ← Final project specification
```

## Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/YOUR_USERNAME/rtes-training.git
cd rtes-training

# 2. Set up the Pi (installs all dependencies)
chmod +x scripts/setup_pi.sh && ./scripts/setup_pi.sh

# 3. Build all exercises
make all

# 4. Run a specific day's exercise (example: Day 6)
cd day06_sched_fifo
sudo taskset -c 0 ./sched_demo

# Or build and run just one day from the top level
make day06
```

## How to Use This Course

1. **Read `TRAINING_PLAN.md`** — the full 25-day plan with theory, context, and detailed expected output
2. **Work through one day at a time** — each day's `README.md` has a focused summary of what to do
3. **Type the code yourself** if you want maximum learning — or compile the provided source
4. **Compare your output** against the expected output documented in each README
5. **Do the challenges** — they push you beyond the basics

## Course Progression

| Week | Days | Theme | Key Skill |
|------|------|-------|-----------|
| 1 | 1–5 | Foundations & Setup | pthreads, mutexes, environment |
| 2 | 6–10 | RT Scheduling | SCHED_FIFO, RMA, jitter measurement |
| 3 | 11–15 | Interference & Sync | Priority inversion, multi-core, sequencer |
| 4 | 16–20 | Camera & Vision | V4L2, OpenCV, frame differencing, WCET |
| 5 | 21–25 | Integration & Test | Synchronome, deadline monitoring, capstone |

## Key Concepts Covered

- **Scheduling:** SCHED_FIFO, SCHED_RR, SCHED_OTHER, priority preemptive run-to-completion
- **Analysis:** Rate Monotonic Analysis, Least Upper Bound, utilization, WCET measurement
- **Synchronization:** Mutexes, semaphores, condition variables, priority inheritance
- **Architecture:** Cyclic executive, sequencer pattern, partitioned multi-core scheduling
- **Camera/Vision:** V4L2, OpenCV C API, frame differencing, motion detection
- **Verification:** Jitter measurement, stress testing, ftrace/trace-cmd, deadline monitoring

## Important Notes

- All RT scheduling exercises require `sudo` (SCHED_FIFO needs root privileges)
- Use `taskset -c 0` to pin to a single core when observing priority preemption
- The semaphore "starting gate" pattern is used throughout — this ensures all threads are created before any start executing (see Day 6 README for the full explanation)

## References

- [POSIX man pages](https://man7.org/linux/man-pages/) — `sched(7)`, `pthread_create(3)`, `clock_nanosleep(2)`
- [V4L2 Documentation](https://www.kernel.org/doc/html/latest/userspace-api/media/v4l/v4l2.html)

## License

This project is released under the MIT License. Use it freely for learning, teaching, and building.
