# Real-Time Embedded Systems — Self-Paced Training Plan
### Hardware: Raspberry Pi 3B + USB Camera | Platform: Linux + POSIX RT Extensions

---

## Pre-Requisites

- Raspberry Pi 3B with Raspbian OS (Bullseye or Bookworm Lite recommended)
- USB Webcam (UVC-compatible)
- SD Card (16 GB+), power supply, SSH or monitor+keyboard access
- Comfort with C programming and basic Linux terminal commands
- **Free textbook reference:** *Real-Time Concepts for Embedded Systems* — Qing Li & Caroline Yao (widely available summaries online)

---

## Week 1 — Foundations & Platform Setup

### Day 1: What Are Real-Time Systems?

**Concepts**
- Definition: deadline-oriented vs. throughput-oriented
- Hard real-time, soft real-time, best-effort
- Examples: ABS brakes (hard), video streaming (soft), web browsing (best-effort)

**Reading**
- Wikipedia: "Real-time computing"
- Search: "Sam Siewert ECEN 5623 real-time concepts slides" (CU Boulder)

**Practical Exercise**

Linux's default scheduler (CFS — Completely Fair Scheduler) divides CPU time equally among all running processes. This is great for desktops but terrible for real-time systems. In this exercise we will saturate all 4 CPU cores with dummy load and use `htop` to visually confirm that Linux treats every process equally — no process gets priority over another. This is the fundamental problem that real-time scheduling solves.

First, we need to install two tools: `stress` (generates artificial CPU load by spawning worker processes that burn CPU cycles) and `htop` (a visual, colour-coded process monitor that shows per-core CPU usage in real time).

```bash
sudo apt update && sudo apt install -y stress htop
```

**Expected output:** A series of package download and install messages ending with something like `Setting up stress ...` and `Setting up htop ...`. If you see `E: Unable to locate package`, check your internet connection and re-run `sudo apt update`.

Now launch 4 CPU-bound stress workers — one per core on the Pi 3B. The `&` sends the command to the background so your terminal stays usable.

```bash
stress --cpu 4 &
```

**Expected output:** `stress: info: [1234] dispatching hogs: 4 cpu, 0 io, 0 vm, 0 hdd` — the number in brackets is the process ID.

Open a second terminal (or a second SSH session) and launch htop. Look at the four CPU bars at the top — all four should be at ~100%, and each stress worker gets roughly equal CPU time. This is fair scheduling in action: the kernel gives every process a "fair share," with no concept of urgency or deadlines.

```bash
htop
```

**Expected output:** A full-screen interactive display. The 4 horizontal bars at the top (labelled 1, 2, 3, 4) should all show nearly 100% utilisation in red/green. You will see 4 `stress` processes in the list, each consuming ~25% total CPU (i.e. ~100% of one core). Press `q` to exit htop.

When you are done observing, kill the stress processes. The `%1` refers to the first backgrounded job in your current shell.

```bash
kill %1
```

**Expected output:** `[1]+  Terminated  stress --cpu 4` (the exact wording may vary slightly).

Write a 1-paragraph reflection: *Why would fair scheduling be dangerous in a pacemaker?*

---

### Day 2: Real-Time Implementation Options Survey

**Concepts**
- Four approaches: (1) Hardware-only, (2) Cyclic Executive, (3) RTOS (e.g. VxWorks, FreeRTOS), (4) General OS + RT extensions
- Why we choose option 4 (Linux + POSIX RT) — convenience, multi-core, broad applicability

**Reading**
- Search: "cyclic executive vs RTOS" — understand trade-offs
- FreeRTOS.org → "What is an RTOS?" page
- Skim POSIX.1b (IEEE 1003.1b) real-time extensions overview

**Practical Exercise**

Before writing any real-time code, we need to confirm that the Pi's Linux kernel actually supports real-time features. Not all kernels are compiled with the same options. We will run three commands that inspect the kernel's build configuration to check for preemption support and POSIX real-time compliance.

This prints the full kernel version string. Look for keywords like "PREEMPT" which indicate the kernel can interrupt (preempt) running tasks — essential for real-time responsiveness.

```bash
uname -a
```

**Expected output:** Something like `Linux raspberrypi 6.1.21-v8+ #1642 SMP PREEMPT Mon Apr  3 17:24:16 BST 2023 aarch64 GNU/Linux`. The key things to verify: (1) `PREEMPT` appears somewhere in the string, and (2) `SMP` is present (symmetric multiprocessing — confirms multi-core support). Your kernel version and date will differ.

This searches the kernel's build configuration file for PREEMPT-related options. `CONFIG_PREEMPT=y` means the kernel supports full preemption — lower priority tasks can be interrupted mid-execution by higher priority tasks. Without this, real-time scheduling has much weaker guarantees.

```bash
cat /boot/config-$(uname -r) | grep PREEMPT
```

**Expected output:** One or more lines like `CONFIG_PREEMPT=y` or `CONFIG_PREEMPT_DYNAMIC=y`. If this file does not exist on your Pi, try `zcat /proc/config.gz | grep PREEMPT` instead (some Pi builds store the config differently). Seeing `CONFIG_PREEMPT_NONE=y` means your kernel is NOT configured for preemption — you would need a different kernel.

This searches the compressed kernel config for all POSIX-related build flags. We want to see options like `CONFIG_POSIX_MQUEUE`, `CONFIG_POSIX_TIMERS`, etc. These confirm that POSIX real-time APIs (the ones we will use throughout this course) are compiled into your kernel.

```bash
zcat /proc/config.gz | grep POSIX
```

**Expected output:** Several lines including `CONFIG_POSIX_MQUEUE=y` and `CONFIG_POSIX_TIMERS=y`. If you get `gzip: /proc/config.gz: No such file or directory`, your kernel was built without embedded config. Try `sudo modprobe configs` first, then re-run. If both approaches fail, do not worry — the Day 3 test program will definitively confirm POSIX RT support.

Create a comparison table (on paper or markdown): Cyclic Exec vs. RTOS vs. Linux+POSIX RT — columns: complexity, determinism, cost, multi-core support.

---

### Day 3: Pi Setup & Linux RT Environment

**Concepts**
- Cross-compilation vs. native compilation
- PREEMPT_RT kernel patch (optional but good to know)
- Essential packages for RT development on Pi

**Practical Exercise — Full Dev Environment Setup**

We need to install all the development tools and libraries that we will use throughout the rest of the course. Here is what each package group does:

`build-essential` provides the GCC C compiler, make, and standard development headers — required to compile any C program on the Pi. `git` is version control for downloading example code repositories. `cmake` is a build system used by many open-source projects including OpenCV. `v4l-utils` provides Video4Linux2 utilities for querying and controlling your USB camera from the command line.

```bash
sudo apt install -y build-essential git cmake v4l-utils
```

**Expected output:** Package download and install messages. If packages are already installed, you will see `build-essential is already the newest version` etc. Either way is fine.

`libpthread-stubs0-dev` provides development headers for POSIX threads (pthreads) — technically often already present, but this ensures the headers are available for linking.

```bash
sudo apt install -y libpthread-stubs0-dev
```

**Expected output:** Either a fresh install or `libpthread-stubs0-dev is already the newest version`.

`fswebcam` is a simple command-line tool to capture a single still image from a webcam — useful for quick camera tests. `ffmpeg` is a powerful multimedia framework we will use to capture video frames from the USB camera. `opencv-data` and `libopencv-dev` provide the OpenCV computer vision library and its C/C++ development headers — we will use this later for frame capture and image processing in C.

```bash
sudo apt install -y fswebcam ffmpeg opencv-data libopencv-dev
```

**Expected output:** Package install messages. OpenCV is large, so this may take a few minutes on the Pi.

Now let us verify the USB camera is detected and working. `lsusb` lists all USB devices connected to the Pi — your camera should appear as a line with the manufacturer name (e.g., "Logitech" or "USB2.0 Camera").

```bash
lsusb
```

**Expected output:** Several lines like:
```
Bus 001 Device 004: ID 046d:0825 Logitech, Inc. Webcam C270
Bus 001 Device 003: ID 0424:ec00 Microchip Technology, Inc.
Bus 001 Device 002: ID 0424:9514 Microchip Technology, Inc. SMC9514 Hub
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
```
Your camera's vendor ID and name will differ. If you do NOT see a camera line, unplug and replug the USB cable, then re-run.

`v4l2-ctl --list-devices` asks the Video4Linux2 subsystem which video capture devices are available. You should see your camera listed with a device path like `/dev/video0`. This is the device file you will open in your code to capture frames.

```bash
v4l2-ctl --list-devices
```

**Expected output:**
```
USB2.0 Camera (usb-3f980000.usb-1.4):
	/dev/video0
	/dev/video1
```
Your camera name and USB path will differ. The important thing is that `/dev/video0` (or similar) appears. If you see `Cannot open device /dev/video0`, the camera driver is not loaded — try `sudo modprobe uvcvideo`.

`fswebcam` takes a single snapshot at 640×480 resolution and saves it as `test.jpg`. If this works, your camera driver, USB connection, and image pipeline are all functional. Open the resulting `test.jpg` to visually verify the image looks correct.

```bash
fswebcam -r 640x480 test.jpg
```

**Expected output:**
```
--- Opening /dev/video0...
Trying source module v4l2...
/dev/video0 opened.
No input was specified, using the first.
--- Capturing frame...
Captured frame in 0.00 seconds.
--- Processing captured image...
Writing JPEG image to 'test.jpg'.
```
The file `test.jpg` should now exist. Verify with `ls -la test.jpg` — it should be roughly 20–80 KB depending on the scene.

Next, we write a tiny C program to verify that the POSIX real-time scheduling API is available and functional. This program calls `sched_get_priority_max()` and `sched_get_priority_min()` for the `SCHED_FIFO` policy. These functions return the range of priority levels your kernel supports for FIFO (real-time) scheduling. If this compiles and runs successfully, your toolchain and kernel are ready for all the RT programming we will do.

```c
// file: test_rt.c
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
int main() {
    int policy_max = sched_get_priority_max(SCHED_FIFO);
    int policy_min = sched_get_priority_min(SCHED_FIFO);
    printf("SCHED_FIFO priority range: %d - %d\n", policy_min, policy_max);
    return 0;
}
```

Compile with `-lpthread` (links the POSIX threads library) and run with `sudo` (SCHED_FIFO queries sometimes require elevated privileges depending on the kernel config).

```bash
gcc test_rt.c -o test_rt -lpthread && sudo ./test_rt
```

**Expected output:**
```
SCHED_FIFO priority range: 1 - 99
```
This means you have 99 usable real-time priority levels (1 = lowest RT, 99 = highest RT). Normal processes run at priority 0 under SCHED_OTHER. If you see `-1 - -1` or an error, your kernel does not support POSIX real-time scheduling and you need a different kernel image.

---

### Day 4: POSIX Threads (pthreads) Basics

**Concepts**
- Thread creation, joining, detaching
- Thread attributes
- Shared memory between threads (vs. processes)

**Practical Exercise — Multi-threaded "Hello"**

Real-time services run as separate threads inside a single process. Before we add scheduling policies, we need to understand the basics: how to create threads, pass arguments to them, and wait for them to finish. This program creates 3 threads, each printing its ID and iteration number. The key observation is that the output will be interleaved unpredictably — the default Linux scheduler makes no guarantees about which thread runs when. This non-determinism is exactly what real-time scheduling will fix later.

- `pthread_create()`: spawns a new thread that starts executing the given function
- `pthread_join()`: blocks the main thread until the specified thread finishes (like `wait()` for processes)
- `usleep(100000)`: pauses for 100 ms to make the interleaving visible to human eyes

```c
// file: pthread_hello.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3

void *thread_func(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 5; i++) {
        printf("Thread %d: iteration %d\n", id, i);
        usleep(100000); // 100 ms
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    printf("All threads done.\n");
    return 0;
}
```

Compile and run. The `-lpthread` flag tells the linker to include the POSIX threads library. No `sudo` is needed because we are not using any real-time scheduling yet — just default fair scheduling.

```bash
gcc pthread_hello.c -o pthread_hello -lpthread && ./pthread_hello
```

**Expected output (order will vary — that's the point!):**
```
Thread 0: iteration 0
Thread 1: iteration 0
Thread 2: iteration 0
Thread 0: iteration 1
Thread 2: iteration 1
Thread 1: iteration 1
Thread 0: iteration 2
Thread 1: iteration 2
Thread 2: iteration 2
Thread 0: iteration 3
Thread 2: iteration 3
Thread 1: iteration 3
Thread 0: iteration 4
Thread 1: iteration 4
Thread 2: iteration 4
All threads done.
```
Run this 3–4 times. The interleaving order will change each run (e.g., sometimes Thread 2 prints before Thread 1). This unpredictability is the core problem RT scheduling addresses. The final line `All threads done.` always appears last because `pthread_join` waits for all threads.

---

### Day 5: POSIX Mutexes & Synchronization

**Concepts**
- Race conditions, critical sections
- `pthread_mutex_t` — lock/unlock
- Deadlock basics

**Practical Exercise — Shared Counter with Race Condition**

When multiple threads access the same variable without protection, you get a race condition. The operation `counter++` looks like one step in C, but in assembly it is three steps: (1) read counter from memory into a register, (2) increment the register, (3) write the register back to memory. If two threads both read the same value before either writes back, one increment is lost. This program demonstrates the problem — and then fixes it with a mutex.

- `pthread_mutex_t`: a mutual exclusion lock — only one thread can hold it at a time
- `PTHREAD_MUTEX_INITIALIZER`: a macro that statically initialises a mutex (no need to call `pthread_mutex_init()`)
- The mutex lines are commented out initially so you can observe the race condition first

```c
// file: race.c
#include <stdio.h>
#include <pthread.h>

int counter = 0;
// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // uncomment to fix

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        // pthread_mutex_lock(&lock);   // uncomment to fix
        counter++;
        // pthread_mutex_unlock(&lock); // uncomment to fix
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter = %d (expected 2000000)\n", counter);
    return 0;
}
```

Compile and run this multiple times without the mutex. You will see a different wrong answer each run because increments are being lost to the race condition.

```bash
gcc race.c -o race -lpthread && ./race
```

**Expected output (WITHOUT mutex — broken):**
```
Counter = 1340287 (expected 2000000)
```
The number will be different every run (e.g., 1567432, 1891003, 1123456) but almost always less than 2000000. Occasionally it may be exactly 2000000 by luck — run it several times to see variation. The lost increments are the race condition in action.

Now uncomment the three mutex lines (the declaration and the lock/unlock calls), recompile, and run again. The counter should now reliably be exactly 2000000 every time, because the mutex ensures only one thread can execute `counter++` at a time.

```bash
gcc race.c -o race -lpthread && ./race
```

**Expected output (WITH mutex — fixed):**
```
Counter = 2000000 (expected 2000000)
```
Run it 5 times — the answer should be 2000000 every single time. The mutex eliminates the race condition. Note: the program will run noticeably slower with the mutex because each thread must acquire and release the lock 1,000,000 times.

---

## Week 2 — RT Scheduling & SCHED_FIFO

### Day 6: Linux Scheduling Policies

**Concepts**
- `SCHED_OTHER` (CFS — fair, default), `SCHED_FIFO` (priority preemptive, run to completion), `SCHED_RR` (round robin with time slice)
- Priority preemptive = highest priority runnable thread ALWAYS runs
- "Run to completion" means a FIFO thread runs until it blocks or a higher priority thread arrives

**Reading**

Read the Linux manual pages for scheduling. These are the authoritative reference for how SCHED_FIFO, SCHED_RR, and SCHED_OTHER behave on your specific kernel.

```bash
man sched
man sched_setscheduler
```

**Expected output:** A scrollable man page. Read the sections on SCHED_FIFO and SCHED_RR carefully. Press `q` to exit.

**Practical Exercise — See the Difference**

This program creates 3 threads with different SCHED_FIFO priorities (10, 50, 90) and gives each one the same CPU-bound workload. Under SCHED_FIFO on a single core, the highest-priority thread (90) will monopolise the CPU until it finishes, then the next highest runs, and so on. This is fundamentally different from fair scheduling where all three would share the CPU equally.

**Critical design point:** We use a semaphore as a "starting gate" so that all three threads are created and waiting BEFORE any of them begins working. Without this, each thread would be created, immediately preempt `main()` (because even priority 10 SCHED_FIFO is higher than main's SCHED_OTHER), run to completion, and finish before the next thread is even created — giving the misleading result of Thread 0 finishing first.

Key APIs:
- `pthread_attr_setschedpolicy(&attr, SCHED_FIFO)`: tells the kernel this thread should use FIFO real-time scheduling instead of the default fair scheduler
- `pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)`: forces the thread to use the scheduling attributes we set, rather than inheriting from the parent thread
- `sem_wait(&start_gate)`: each thread blocks here until main releases it — ensures all threads compete simultaneously

```c
// file: sched_demo.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>

sem_t start_gate;

void *worker(void *arg) {
    int id = *(int *)arg;

    // Block here until main() releases all threads simultaneously.
    // Without this gate, each thread would preempt main() immediately
    // upon creation (SCHED_FIFO > SCHED_OTHER) and run to completion
    // before the next thread is even created.
    sem_wait(&start_gate);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Busy work — a CPU-bound loop that takes a measurable amount of time
    volatile long sum = 0;
    for (long i = 0; i < 50000000L; i++) sum += i;

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Thread %d (prio %d): %.4f sec, started at %.4f, ended at %.4f\n",
           id, id == 0 ? 10 : id == 1 ? 50 : 90,
           elapsed,
           start.tv_sec + start.tv_nsec / 1e9,
           end.tv_sec + end.tv_nsec / 1e9);
    return NULL;
}

int main() {
    sem_init(&start_gate, 0, 0);

    pthread_t t[3];
    pthread_attr_t attr;
    struct sched_param param;
    int ids[] = {0, 1, 2};
    int prios[] = {10, 50, 90}; // low, mid, high

    // Step 1: Create all threads — they immediately block on sem_wait
    for (int i = 0; i < 3; i++) {
        pthread_attr_init(&attr);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        param.sched_priority = prios[i];
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&t[i], &attr, worker, &ids[i]);
    }

    // Step 2: Release all threads simultaneously.
    // Now all 3 exist and compete — highest priority (90) runs first.
    for (int i = 0; i < 3; i++)
        sem_post(&start_gate);

    for (int i = 0; i < 3; i++) pthread_join(t[i], NULL);
    sem_destroy(&start_gate);
    return 0;
}
```

Compile and run with `sudo` — SCHED_FIFO requires root privileges because it can starve other processes (including the OS itself) if misused.

```bash
gcc sched_demo.c -o sched_demo -lpthread -lrt
sudo ./sched_demo
```

**Expected output (multi-core — all 4 cores available):**
```
Thread 2 (prio 90): 0.4650 sec, started at 1826.1150, ended at 1826.5800
Thread 1 (prio 50): 0.4640 sec, started at 1826.1152, ended at 1826.5792
Thread 0 (prio 10): 0.4630 sec, started at 1826.1155, ended at 1826.5785
```
With multiple cores, all threads may run in parallel on separate cores, so they all start and finish at roughly the same time. The priority effect is not clearly visible because there is no contention. The output order may vary.

Now run again pinned to a single core using `taskset -c 0`. This restricts all 3 threads to core 0, making the priority preemption effect obvious: the highest-priority thread monopolises the CPU, finishes first, then the next runs.

```bash
sudo taskset -c 0 ./sched_demo
```

**Expected output (single core — priority preemption visible):**
```
Thread 2 (prio 90): 0.4650 sec, started at 1826.1150, ended at 1826.5800
Thread 1 (prio 50): 0.4640 sec, started at 1826.5801, ended at 1827.0441
Thread 0 (prio 10): 0.4630 sec, started at 1827.0442, ended at 1827.5072
```
**Key things to verify:**
- Thread 2 (priority 90) starts first and finishes first
- Thread 1 (priority 50) starts only AFTER Thread 2 ends (its start time ≈ Thread 2's end time)
- Thread 0 (priority 10) starts only AFTER Thread 1 ends
- Each thread's elapsed time is similar (~0.46 sec) — the workload is the same
- Total wall-clock time is ~3× a single thread's time (they run serially, not in parallel)

This is SCHED_FIFO in action: the highest-priority runnable thread always gets the CPU. Lower-priority threads are completely starved until higher ones voluntarily release the CPU (by finishing or blocking).

---

### Day 7: POSIX Real-Time Clock & Timing

**Concepts**
- `CLOCK_MONOTONIC` vs `CLOCK_REALTIME`
- `clock_gettime()`, `clock_nanosleep()`
- Jitter: measuring the gap between expected and actual wake-up

**Practical Exercise — Jitter Measurement**

In a real-time system, a service must wake up at precise intervals (e.g., exactly every 10 ms). The difference between when a thread *should* wake up and when it *actually* wakes up is called **jitter**. This program measures jitter by scheduling itself as a high-priority SCHED_FIFO process, sleeping for exactly 10 ms periods using `clock_nanosleep()` with an absolute time target, and then checking how late it actually woke up.

Key design choices:
- `CLOCK_MONOTONIC`: a clock that never jumps backwards and is not affected by NTP adjustments — essential for measuring real elapsed time
- `TIMER_ABSTIME`: we compute the *absolute* next wake-up time rather than a relative sleep duration — this prevents drift (if we overran slightly, the next wake-up is still correct)
- Priority 80: high enough to preempt most other processes but leaves room for even higher priority services

```c
// file: jitter.c
#include <stdio.h>
#include <time.h>
#include <sched.h>

#define ITERATIONS 100
#define PERIOD_MS 10  // 10 ms = 100 Hz

int main() {
    struct sched_param param = { .sched_priority = 80 };
    sched_setscheduler(0, SCHED_FIFO, &param);

    struct timespec next, now;
    double jitters[ITERATIONS];
    clock_gettime(CLOCK_MONOTONIC, &next);

    for (int i = 0; i < ITERATIONS; i++) {
        // Calculate the absolute time of the next wake-up
        next.tv_nsec += PERIOD_MS * 1000000L;
        if (next.tv_nsec >= 1000000000L) {
            next.tv_sec++;
            next.tv_nsec -= 1000000000L;
        }
        // Sleep until that absolute time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        // Measure how late we actually woke up
        clock_gettime(CLOCK_MONOTONIC, &now);

        long diff_ns = (now.tv_sec - next.tv_sec) * 1000000000L
                     + (now.tv_nsec - next.tv_nsec);
        jitters[i] = diff_ns / 1000.0; // convert to microseconds
    }

    double max_j = 0, avg_j = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        if (jitters[i] > max_j) max_j = jitters[i];
        avg_j += jitters[i];
    }
    avg_j /= ITERATIONS;
    printf("Jitter over %d iterations at %d ms period:\n", ITERATIONS, PERIOD_MS);
    printf("  Avg: %.1f us, Max: %.1f us\n", avg_j, max_j);
    return 0;
}
```

Compile with `-lrt` (links the POSIX real-time library which provides `clock_nanosleep`). Run with `sudo` because SCHED_FIFO requires root.

```bash
gcc jitter.c -o jitter -lrt
sudo ./jitter
```

**Expected output (no background load):**
```
Jitter over 100 iterations at 10 ms period:
  Avg: 15.3 us, Max: 85.2 us
```
Typical values on a Pi 3B: average jitter of 10–50 µs, max jitter of 50–200 µs. These are microseconds — well within most real-time requirements. If your max jitter exceeds 1000 µs (1 ms), something else is consuming CPU.

Now run the same test again, but this time with heavy background CPU load. The `stress --cpu 4` command saturates all cores. Because our process runs at SCHED_FIFO priority 80, it should still wake up on time — the jitter should not increase dramatically. This demonstrates the power of real-time scheduling: our deadline-critical task preempts the stress workers.

```bash
stress --cpu 4 &
sudo ./jitter
kill %1
```

**Expected output (with background load):**
```
Jitter over 100 iterations at 10 ms period:
  Avg: 18.7 us, Max: 120.4 us
```
The numbers should be in the same ballpark as the no-load run. SCHED_FIFO at priority 80 preempts the normal-priority stress workers, so our timer precision is maintained even under full CPU load. If jitter increased dramatically (10× or more), your kernel may not have full preemption support.

---

### Day 8: Cyclic Executive — Build One

**Concepts**
- Simplest RT architecture: a timed loop executing services in a fixed sequence
- No OS needed — but fragile and hard to scale
- Major frame, minor frame

**Practical Exercise — 3-Service Cyclic Executive**

A cyclic executive is the simplest possible real-time scheduler: a single loop that runs at a fixed rate and calls each service in a predetermined order. There are no threads, no preemption, and no OS scheduler involved. The programmer manually decides which services run in each time slot.

In this example, the "major frame" is 30 iterations and the "minor frame" is 10 ms. Service 1 (sensor read) runs every minor frame, Service 2 (processing) runs every 2nd frame, and Service 3 (logging) runs every 3rd frame. This is the approach used in many older avionics and automotive systems.

The advantage is absolute determinism — you know exactly what runs when. The disadvantage is rigidity — adding a new service requires reworking the entire schedule, and if any service overruns its slot, all subsequent services are delayed.

```c
// file: cyclic_exec.c
#include <stdio.h>
#include <time.h>
#include <sched.h>

void service1() { printf("[S1] Sensor read   @ "); }
void service2() { printf("[S2] Processing    @ "); }
void service3() { printf("[S3] Logging       @ "); }

void print_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    printf("%ld.%06ld\n", ts.tv_sec, ts.tv_nsec / 1000);
}

int main() {
    // Set ourselves to highest RT priority so Linux does not preempt us
    struct sched_param p = { .sched_priority = 90 };
    sched_setscheduler(0, SCHED_FIFO, &p);

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    // Major frame = 30 iterations, minor frame = 10 ms
    // S1 runs every minor frame, S2 every 2nd, S3 every 3rd
    for (int frame = 0; frame < 30; frame++) {
        service1(); print_time();

        if (frame % 2 == 0) { service2(); print_time(); }
        if (frame % 3 == 0) { service3(); print_time(); }

        // Sleep until next minor frame boundary
        next.tv_nsec += 10000000L; // 10 ms
        if (next.tv_nsec >= 1000000000L) { next.tv_sec++; next.tv_nsec -= 1000000000L; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return 0;
}
```

Compile with `-lrt` for the real-time clock functions and run as root for SCHED_FIFO.

```bash
gcc cyclic_exec.c -o cyclic_exec -lrt && sudo ./cyclic_exec
```

**Expected output (first 10 lines shown):**
```
[S1] Sensor read   @ 1830.123456
[S2] Processing    @ 1830.123478
[S3] Logging       @ 1830.123490
[S1] Sensor read   @ 1830.133502
[S1] Sensor read   @ 1830.143510
[S2] Processing    @ 1830.143525
[S3] Logging       @ 1830.143535
[S1] Sensor read   @ 1830.153548
[S1] Sensor read   @ 1830.163555
[S2] Processing    @ 1830.163570
```
**Key things to verify:**
- S1 appears in every frame (every ~10 ms — timestamps should increment by ~0.010)
- S2 appears every 2nd frame (frames 0, 2, 4, ...)
- S3 appears every 3rd frame (frames 0, 3, 6, ...)
- Frame 0 has all three: S1, S2, and S3
- Frame 6 also has all three (LCM of 2 and 3)
- The timestamps are monotonically increasing and evenly spaced

**Challenge:** Add a 4th service that runs every 5th frame. Observe what happens if a service overruns its time slot (add a `usleep(15000)` inside service1 to simulate a 15ms overrun in a 10ms slot).

---

### Day 9: Rate Monotonic Scheduling — Theory

**Concepts**
- Rate Monotonic Analysis (RMA): assign priorities by rate — fastest period = highest priority
- Schedulability: all tasks meet deadlines if total utilization ≤ Least Upper Bound (LUB)
- LUB for n tasks = n(2^(1/n) - 1) → approaches ln(2) ≈ 0.693 as n → ∞
- For 2 tasks: LUB = 0.828, for 3 tasks: LUB = 0.780

**Practical Exercise — Hand Calculation**

Rate Monotonic Analysis gives us a mathematical test: if the total CPU utilization of all tasks is below the Least Upper Bound (LUB), then RMA *guarantees* all deadlines will be met. Utilization for each task is WCET (worst-case execution time) divided by period.

```
Task    Period (ms)    WCET (ms)    Utilization
T1      10             2            0.200
T2      20             4            0.200
T3      50             10           0.200
                            Total:  0.600

LUB for 3 tasks = 3(2^(1/3) - 1) = 0.780
0.600 < 0.780 → SCHEDULABLE under RMA ✓
```

**Exercises (do these on paper — answers below):**
1. What is the max WCET for T3 before the system becomes unschedulable?
2. Add a T4 with period 100 ms. What is the new LUB for 4 tasks? What max WCET can T4 have?
3. Create a scenario where utilization is below 1.0 but above the LUB — is it necessarily unschedulable?

**Answers:**
1. Total U must stay ≤ 0.780. T1+T2 use 0.400, so T3 can use up to 0.380. WCET_max = 0.380 × 50 = **19 ms**.
2. LUB for 4 tasks = 4(2^(1/4) - 1) = 4 × 0.1892 = **0.757**. T1+T2+T3 use 0.600, so T4 can use up to 0.157. WCET_max = 0.157 × 100 = **15.7 ms**.
3. Not necessarily unschedulable — the LUB is a **sufficient** condition, not necessary. A system above the LUB but below 1.0 *might* still be schedulable — you need exact analysis (response-time analysis) to determine this.

---

### Day 10: Rate Monotonic Scheduling — Implementation

**Practical Exercise — 3-Service RMS on Pi**

Now we implement in code what we calculated by hand on Day 9. This program creates 3 real-time threads with periods of 10 ms, 20 ms, and 50 ms. Following the Rate Monotonic rule, the fastest task (T1, 10 ms period) gets the highest priority (90), the next fastest (T2, 20 ms) gets priority 80, and the slowest (T3, 50 ms) gets priority 70.

Key implementation details:
- `sem_t start_sem`: a semaphore used as a starting gate — all threads are created first, then released simultaneously so they begin executing at the same baseline time. This is critical for the same reason as Day 6: without it, each thread would preempt main() upon creation and run before the next thread exists.
- `busy_work_ms()`: simulates a task's actual computation by busy-looping for a specified number of milliseconds (measuring elapsed time to be accurate)
- Each thread sleeps using `clock_nanosleep` with absolute time targets to maintain its periodic rate

```c
// file: rms_demo.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

sem_t start_sem;

typedef struct {
    int id;
    int period_ms;
    int wcet_ms;
    int priority;
    int iterations;
} task_params_t;

// Simulates real computation by busy-looping for exactly 'ms' milliseconds.
// We use clock_gettime rather than usleep because usleep would yield the CPU
// (we want to model actual CPU-bound work that consumes processing time).
void busy_work_ms(int ms) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed = (now.tv_sec - start.tv_sec) * 1000 +
                       (now.tv_nsec - start.tv_nsec) / 1000000;
        if (elapsed >= ms) break;
    }
}

void *rt_task(void *arg) {
    task_params_t *p = (task_params_t *)arg;
    sem_wait(&start_sem); // all threads wait here until main releases them

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    for (int i = 0; i < p->iterations; i++) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        printf("T%d start @ %ld.%03ld\n", p->id,
               ts.tv_sec % 100, ts.tv_nsec / 1000000);

        busy_work_ms(p->wcet_ms);

        // Advance to next period boundary
        next.tv_nsec += p->period_ms * 1000000L;
        while (next.tv_nsec >= 1000000000L) {
            next.tv_sec++;
            next.tv_nsec -= 1000000000L;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}

int main() {
    sem_init(&start_sem, 0, 0);

    // RMA: fastest rate → highest priority
    task_params_t tasks[] = {
        { .id=1, .period_ms=10,  .wcet_ms=2,  .priority=90, .iterations=100 },
        { .id=2, .period_ms=20,  .wcet_ms=4,  .priority=80, .iterations=50  },
        { .id=3, .period_ms=50,  .wcet_ms=10, .priority=70, .iterations=20  },
    };
    int n = 3;
    pthread_t threads[3];

    for (int i = 0; i < n; i++) {
        pthread_attr_t attr;
        struct sched_param sp = { .sched_priority = tasks[i].priority };
        pthread_attr_init(&attr);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &sp);
        pthread_create(&threads[i], &attr, rt_task, &tasks[i]);
    }

    // Release all threads simultaneously
    for (int i = 0; i < n; i++) sem_post(&start_sem);

    for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);
    sem_destroy(&start_sem);
    return 0;
}
```

Compile with both `-lpthread` (for thread and semaphore functions) and `-lrt` (for clock functions). Pin to a single core with `taskset -c 0` so that preemption behaviour is clearly visible — on multiple cores, tasks can run in parallel which obscures the priority ordering.

```bash
gcc rms_demo.c -o rms_demo -lpthread -lrt
sudo taskset -c 0 ./rms_demo
```

**Expected output (first ~15 lines, single core):**
```
T1 start @ 30.100
T1 start @ 30.110
T2 start @ 30.112
T1 start @ 30.120
T1 start @ 30.130
T2 start @ 30.132
T1 start @ 30.140
T1 start @ 30.150
T2 start @ 30.152
T3 start @ 30.156
T1 start @ 30.160
```
**Key things to verify:**
- T1 appears most frequently (every ~10 ms) because it has the fastest period and highest priority
- T2 appears roughly every ~20 ms, but always *after* T1 finishes its 2 ms of work in that frame
- T3 appears roughly every ~50 ms, after both T1 and T2 have finished
- T1 is never delayed by T2 or T3 — it always starts on time
- The timestamps increment in ~10 ms steps

**Challenge:** Gradually increase T3's `wcet_ms` from 10 to 15 to 20. At some point, T3 (or T2) will start missing deadlines. Verify this matches your LUB calculation from Day 9.

---

## Week 3 — Interference, Blocking & Multi-Core

### Day 11: Interference vs. Blocking

**Concepts**
- **Interference:** higher-priority task preempts your task (you cannot prevent it)
- **Blocking:** lower-priority task holds a resource you need (priority inversion!)
- Priority Inheritance Protocol (PIP): temporarily boost blocker's priority
- Priority Ceiling Protocol (PCP): prevent deadlocks

**Practical Exercise — Priority Inversion Demo**

Priority inversion is one of the most dangerous problems in real-time systems. It occurs when a high-priority task is blocked waiting for a resource held by a low-priority task, and a medium-priority task preempts the low-priority task — making the high-priority task wait even longer. This famously caused the Mars Pathfinder rover to repeatedly reset in 1997.

In this program:
- A low-priority thread (priority 10) locks a mutex and does 200 ms of work while holding it
- A high-priority thread (priority 90) starts 50 ms later and tries to lock the same mutex
- The high-priority thread is blocked until the low-priority thread finishes — a priority inversion

The fix is `PTHREAD_PRIO_INHERIT`: when the high-priority thread blocks on the mutex, the kernel temporarily raises the low-priority thread's effective priority to 90, ensuring it finishes quickly and releases the mutex.

```c
// file: priority_inversion.c
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t shared_resource;

void busy_ms(int ms) {
    struct timespec s, n;
    clock_gettime(CLOCK_MONOTONIC, &s);
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &n);
        if ((n.tv_sec-s.tv_sec)*1000 + (n.tv_nsec-s.tv_nsec)/1000000 >= ms) break;
    }
}

void *low_prio(void *arg) {
    printf("LOW: locking mutex\n");
    pthread_mutex_lock(&shared_resource);
    printf("LOW: got mutex, working 200ms...\n");
    busy_ms(200);
    printf("LOW: releasing mutex\n");
    pthread_mutex_unlock(&shared_resource);
    return NULL;
}

void *high_prio(void *arg) {
    usleep(50000); // start 50ms late so low_prio grabs the mutex first
    printf("HIGH: trying to lock mutex...\n");
    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pthread_mutex_lock(&shared_resource);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    long wait_ms = (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_nsec-t1.tv_nsec)/1000000;
    printf("HIGH: got mutex after %ld ms (blocked!)\n", wait_ms);
    pthread_mutex_unlock(&shared_resource);
    return NULL;
}

int main() {
    // PTHREAD_PRIO_INHERIT is the fix for priority inversion.
    // Try first WITHOUT it (comment out), then WITH it (uncomment).
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    // pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT); // uncomment to fix
    pthread_mutex_init(&shared_resource, &mattr);

    pthread_t tl, th;
    pthread_attr_t attr;
    struct sched_param sp;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    sp.sched_priority = 10;
    pthread_attr_setschedparam(&attr, &sp);
    pthread_create(&tl, &attr, low_prio, NULL);

    sp.sched_priority = 90;
    pthread_attr_setschedparam(&attr, &sp);
    pthread_create(&th, &attr, high_prio, NULL);

    pthread_join(tl, NULL);
    pthread_join(th, NULL);
    return 0;
}
```

Pin to a single core to make the blocking clearly visible.

```bash
gcc priority_inversion.c -o prio_inv -lpthread -lrt
sudo taskset -c 0 ./prio_inv
```

**Expected output (WITHOUT PTHREAD_PRIO_INHERIT):**
```
LOW: locking mutex
LOW: got mutex, working 200ms...
HIGH: trying to lock mutex...
LOW: releasing mutex
HIGH: got mutex after 150 ms (blocked!)
```
The high-priority thread had to wait ~150 ms (the remaining time of LOW's 200 ms work after HIGH started at the 50 ms mark). During this wait, the high-priority thread was blocked by a lower-priority thread — this is the priority inversion.

**Expected output (WITH PTHREAD_PRIO_INHERIT uncommented):**
```
LOW: locking mutex
LOW: got mutex, working 200ms...
HIGH: trying to lock mutex...
LOW: releasing mutex
HIGH: got mutex after 150 ms (blocked!)
```
The wait time will be similar (~150 ms) in this simple two-thread case because there is no medium-priority task causing the classic unbounded inversion. The key difference shows up when you add a medium-priority thread (priority 50) that does CPU work — without inheritance, the medium thread preempts LOW, extending HIGH's wait. With inheritance, LOW is boosted to priority 90, so the medium thread cannot preempt it.

---

### Day 12: Multi-Core RT Considerations

**Concepts**
- RPi 3B has 4 ARM Cortex-A53 cores
- Partitioned vs. global scheduling
- CPU affinity: pin services to specific cores
- Cache effects and memory contention between cores

**Practical Exercise — CPU Affinity & Partitioned Scheduling**

On a multi-core system, the OS can migrate threads between cores at any time. This causes unpredictable cache misses and makes timing analysis harder. "Partitioned scheduling" means we manually assign (pin) each real-time task to a specific core and never let it migrate. This makes the system behave like multiple independent single-core systems, which is much easier to analyze.

Key new APIs:
- `CPU_ZERO(&cpuset)`: initialises an empty CPU set
- `CPU_SET(i, &cpuset)`: adds core `i` to the set
- `pthread_attr_setaffinity_np()`: pins the thread to only run on the cores in the set
- `sched_getcpu()`: returns which core the calling thread is currently running on

The `_GNU_SOURCE` define at the top is required because `pthread_attr_setaffinity_np` is a GNU/Linux extension, not part of the POSIX standard.

```c
// file: multicore_rt.c
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

void busy_ms(int ms) {
    struct timespec s, n;
    clock_gettime(CLOCK_MONOTONIC, &s);
    while(1) {
        clock_gettime(CLOCK_MONOTONIC, &n);
        if ((n.tv_sec-s.tv_sec)*1000+(n.tv_nsec-s.tv_nsec)/1000000 >= ms) break;
    }
}

void *task(void *arg) {
    int id = *(int*)arg;
    int cpu = sched_getcpu();
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    busy_ms(50);
    clock_gettime(CLOCK_MONOTONIC, &end);
    long ms = (end.tv_sec-start.tv_sec)*1000+(end.tv_nsec-start.tv_nsec)/1000000;
    printf("Task %d ran on CPU %d, took %ld ms\n", id, cpu, ms);
    return NULL;
}

int main() {
    pthread_t t[4];
    int ids[4];

    for (int i = 0; i < 4; i++) {
        ids[i] = i;
        pthread_attr_t attr;
        struct sched_param sp = { .sched_priority = 80 };
        cpu_set_t cpuset;

        pthread_attr_init(&attr);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &sp);

        // Pin each task to its own core (core 0, 1, 2, 3)
        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        pthread_attr_setaffinity_np(&attr, sizeof(cpuset), &cpuset);

        pthread_create(&t[i], &attr, task, &ids[i]);
    }
    for (int i = 0; i < 4; i++) pthread_join(t[i], NULL);
    return 0;
}
```

Run this and observe that each task reports running on its assigned core.

```bash
gcc multicore_rt.c -o multicore_rt -lpthread -lrt
sudo ./multicore_rt
```

**Expected output (4 tasks on 4 separate cores):**
```
Task 0 ran on CPU 0, took 50 ms
Task 1 ran on CPU 1, took 50 ms
Task 2 ran on CPU 2, took 50 ms
Task 3 ran on CPU 3, took 50 ms
```
**Key things to verify:**
- Each task reports a different CPU number (0, 1, 2, 3)
- All tasks took ~50 ms each
- Total wall-clock time is ~50 ms (all ran in parallel)
- Output order may vary (the tasks finish at nearly the same time)

**Challenge:** Change `CPU_SET(i, ...)` to `CPU_SET(0, ...)` for all 4 tasks, recompile, and run again.

**Expected output (4 tasks all on CPU 0):**
```
Task 0 ran on CPU 0, took 50 ms
Task 1 ran on CPU 0, took 50 ms
Task 2 ran on CPU 0, took 50 ms
Task 3 ran on CPU 0, took 50 ms
```
All tasks report CPU 0 now. Total wall-clock time jumps to ~200 ms because they run serially on one core. This demonstrates why partitioning decisions directly affect schedulability.

---

### Day 13: POSIX Semaphores & Condition Variables

**Concepts**
- Binary semaphore as event signal
- Counting semaphore for resource pools
- `sem_post()` / `sem_wait()` — producer/consumer
- Condition variables for more complex signaling

**Practical Exercise — Producer/Consumer with Semaphores**

The producer/consumer pattern is fundamental in real-time systems: one service produces data (e.g., camera frames) and another consumes it (e.g., image processing). We need to coordinate them without race conditions and without busy-waiting.

This program uses two semaphores:
- `empty`: counts available buffer slots (starts at BUFFER_SIZE = 5). The producer waits on this before writing — if the buffer is full, it blocks.
- `full`: counts filled buffer slots (starts at 0). The consumer waits on this before reading — if the buffer is empty, it blocks.
- A mutex protects the actual buffer array during read/write since semaphores only handle counting, not mutual exclusion on the data structure.

```c
// file: producer_consumer.c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <unistd.h>

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int in_idx = 0, out_idx = 0;

sem_t empty, full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&empty);              // wait for an empty slot
        pthread_mutex_lock(&mutex);    // protect buffer access
        buffer[in_idx] = i;
        printf("Produced: %d at slot %d\n", i, in_idx);
        in_idx = (in_idx + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);               // signal that a new item is available
        usleep(50000);                 // produce every 50ms
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&full);               // wait for a filled slot
        pthread_mutex_lock(&mutex);
        int val = buffer[out_idx];
        printf("  Consumed: %d from slot %d\n", val, out_idx);
        out_idx = (out_idx + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);              // signal that a slot is now free
        usleep(80000);                 // consume every 80ms (slower than producer)
    }
    return NULL;
}

int main() {
    sem_init(&empty, 0, BUFFER_SIZE); // 5 empty slots initially
    sem_init(&full, 0, 0);           // 0 filled slots initially
    pthread_t p, c;
    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    return 0;
}
```

Compile and run. Notice how the producer fills the buffer faster than the consumer empties it — eventually the producer blocks on `sem_wait(&empty)` when all 5 slots are full, and only resumes when the consumer frees a slot.

```bash
gcc producer_consumer.c -o prodcon -lpthread && ./prodcon
```

**Expected output (interleaving varies, but the pattern is consistent):**
```
Produced: 0 at slot 0
Produced: 1 at slot 1
  Consumed: 0 from slot 0
Produced: 2 at slot 2
Produced: 3 at slot 3
  Consumed: 1 from slot 1
Produced: 4 at slot 4
Produced: 5 at slot 0
  Consumed: 2 from slot 2
Produced: 6 at slot 1
  Consumed: 3 from slot 3
...
  Consumed: 19 from slot 4
```
**Key things to verify:**
- Items are consumed in the same order they were produced (0, 1, 2, ... 19) — FIFO ordering
- The producer is faster (50 ms) than the consumer (80 ms), so it initially gets ahead
- After the buffer fills (5 items), the producer must wait for the consumer before producing more
- Slot numbers cycle through 0→1→2→3→4→0→1... (circular buffer)
- All 20 items are produced and all 20 are consumed

---

### Day 14: POSIX Timers & Interval Services

**Concepts**
- `timer_create()`, `timer_settime()` with `SIGALRM` or thread notification
- Periodic services using `CLOCK_MONOTONIC`
- Comparing `clock_nanosleep` approach vs. POSIX timer approach

**Practical Exercise — Timer-Driven Periodic Service**

Instead of a thread calling `clock_nanosleep()` in a loop (pull model), we can use POSIX timers to have the kernel send us a signal at regular intervals (push model). This program creates a timer that fires every 10 ms, triggering a `SIGALRM` signal. The signal handler runs automatically — we do not need to manage sleep/wake timing ourselves.

- `timer_create()`: creates a per-process timer linked to `CLOCK_MONOTONIC`
- `SIGEV_SIGNAL`: tells the timer to deliver a signal when it expires
- `it_interval`: the repeating interval (10 ms)
- `it_value`: the initial delay before the first fire (also 10 ms here)

```c
// file: posix_timer.c
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

volatile int count = 0;

void timer_handler(int sig) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    printf("Tick %d @ %ld.%06ld\n", count++,
           ts.tv_sec % 100, ts.tv_nsec / 1000);
}

int main() {
    // Set RT priority so our signal handler is not delayed by other processes
    struct sched_param p = { .sched_priority = 80 };
    sched_setscheduler(0, SCHED_FIFO, &p);

    // Register the signal handler
    signal(SIGALRM, timer_handler);

    // Create a timer that sends SIGALRM on expiry
    timer_t tid;
    struct sigevent sev = { .sigev_notify = SIGEV_SIGNAL, .sigev_signo = SIGALRM };
    timer_create(CLOCK_MONOTONIC, &sev, &tid);

    // Set the timer: first fire in 10ms, then repeat every 10ms
    struct itimerspec its = {
        .it_interval = { .tv_sec = 0, .tv_nsec = 10000000 }, // 10 ms repeat
        .it_value    = { .tv_sec = 0, .tv_nsec = 10000000 }  // 10 ms initial
    };
    timer_settime(tid, 0, &its, NULL);

    sleep(1); // run for 1 second → expect ~100 ticks
    printf("Total ticks: %d\n", count);
    return 0;
}
```

Compile with `-lrt` (provides `timer_create` and related functions) and run as root.

```bash
gcc posix_timer.c -o posix_timer -lrt
sudo ./posix_timer
```

**Expected output (last few lines shown):**
```
Tick 95 @ 31.073456
Tick 96 @ 31.083478
Tick 97 @ 31.093490
Tick 98 @ 31.103502
Tick 99 @ 31.113510
Total ticks: 100
```
**Key things to verify:**
- Total ticks should be ~100 (100 × 10 ms = 1 second)
- Timestamps should increment by ~0.010 (10 ms) between each tick
- If total ticks is significantly less than 100 (e.g., 50), signals may be getting coalesced — a limitation of signal-based timers under load

---

### Day 15: Week 1-2 Integration — Sequencer Pattern

**Concepts**
- Sequencer: a high-priority timer thread that releases services at their periods
- This is the bridge between cyclic executive and full RMS
- Used throughout Sam Siewert's course examples

**Practical Exercise — Build a Sequencer**

The sequencer pattern is the backbone architecture for the rest of this course. It combines the fixed-schedule predictability of a cyclic executive with the multi-threaded flexibility of RMS. Here is how it works:

1. A single "sequencer" thread runs at the highest priority (99) and wakes up every 10 ms (the base rate)
2. Each service runs in its own thread, blocked on a semaphore
3. The sequencer decides which services to release each frame by posting their semaphores
4. Services run at their own SCHED_FIFO priorities (assigned by RMA — fastest rate = highest priority)

In this example: S1 runs every frame (100 Hz), S2 every 2nd frame (50 Hz), S3 every 5th frame (20 Hz).

```c
// file: sequencer.c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>

#define SEQ_PERIOD_MS 10
#define NUM_FRAMES    100 // 1 second total

sem_t sem_s1, sem_s2, sem_s3;
volatile int abort_test = 0;

void *service(void *arg) {
    int id = *(int*)arg;
    sem_t *sem = (id==1) ? &sem_s1 : (id==2) ? &sem_s2 : &sem_s3;
    while (!abort_test) {
        sem_wait(sem);    // block until sequencer releases us
        if (abort_test) break;
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        printf("S%d @ %ld.%03ld\n", id, ts.tv_sec%100, ts.tv_nsec/1000000);
    }
    return NULL;
}

int main() {
    // Sequencer runs at highest RT priority
    struct sched_param p = { .sched_priority = 99 };
    sched_setscheduler(0, SCHED_FIFO, &p);

    sem_init(&sem_s1, 0, 0);
    sem_init(&sem_s2, 0, 0);
    sem_init(&sem_s3, 0, 0);

    int ids[] = {1, 2, 3};
    int prios[] = {90, 80, 70}; // RMA: S1 fastest → highest
    pthread_t threads[3];

    for (int i = 0; i < 3; i++) {
        pthread_attr_t attr;
        struct sched_param sp = { .sched_priority = prios[i] };
        pthread_attr_init(&attr);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &sp);
        pthread_create(&threads[i], &attr, service, &ids[i]);
    }

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    for (int frame = 0; frame < NUM_FRAMES; frame++) {
        sem_post(&sem_s1);                        // release S1 every frame
        if (frame % 2 == 0) sem_post(&sem_s2);    // release S2 every 2nd frame
        if (frame % 5 == 0) sem_post(&sem_s3);    // release S3 every 5th frame

        // Sleep until next frame boundary
        next.tv_nsec += SEQ_PERIOD_MS * 1000000L;
        if (next.tv_nsec >= 1000000000L) { next.tv_sec++; next.tv_nsec -= 1000000000L; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }

    // Signal all services to exit by setting the abort flag and unblocking them
    abort_test = 1;
    sem_post(&sem_s1); sem_post(&sem_s2); sem_post(&sem_s3);
    for (int i = 0; i < 3; i++) pthread_join(threads[i], NULL);
    return 0;
}
```

Pin to a single core to observe pure priority-preemptive behaviour. The `| head -40` limits output so you can study the first few frames without being overwhelmed.

```bash
gcc sequencer.c -o sequencer -lpthread -lrt
sudo taskset -c 0 ./sequencer | head -40
```

**Expected output (first ~20 lines):**
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
S1 @ 45.160
S2 @ 45.160
S1 @ 45.170
S1 @ 45.180
S2 @ 45.180
```
**Key things to verify:**
- Frame 0: S1, S2, and S3 all fire (all conditions met: every frame, every 2nd, every 5th)
- S1 appears every ~10 ms (every frame)
- S2 appears every ~20 ms (frames 0, 2, 4, ...)
- S3 appears every ~50 ms (frames 0, 5, 10, ...)
- When multiple services fire in the same frame, S1 prints first (highest priority 90), then S2 (80), then S3 (70)
- Timestamps increment by ~10 ms between frames

---

## Week 4 — Camera Integration & Machine Vision

### Day 16: V4L2 — USB Camera on Linux

**Concepts**
- Video4Linux2 (V4L2) API — the standard Linux camera interface
- Frame capture, resolution, format negotiation
- Frame rate vs. exposure

**Practical Exercise — Camera Enumeration & Capture**

Before writing camera code, we need to understand what our camera can do. The V4L2 utilities let us query the camera's supported resolutions, pixel formats, and frame rates without writing any code.

This command queries all properties of the camera device at `/dev/video0` — including driver name, capabilities, current resolution, brightness/contrast controls, and more. It is your first stop for understanding what hardware you are working with.

```bash
v4l2-ctl -d /dev/video0 --all
```

**Expected output:** Multiple sections including `Driver Info`, `Video input`, `Format Video Capture`, and `User Controls`. Look for the current resolution (e.g., `Width/Height: 640/480`) and available controls (brightness, contrast, etc.). The output will be 30–50 lines long.

This lists all pixel formats (MJPEG, YUYV, etc.) and for each format, all supported resolutions and frame rates. You need this to know what frame rates are achievable.

```bash
v4l2-ctl --list-formats-ext
```

**Expected output:**
```
ioctl: VIDIOC_ENUM_FMT
	Type: Video Capture

	[0]: 'YUYV' (YUYV 4:2:2)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 320x240
			Interval: Discrete 0.033s (30.000 fps)
	[1]: 'MJPG' (Motion-JPEG, compressed)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
```
Your camera's supported formats will differ. MJPEG typically supports higher frame rates at higher resolutions because the data is compressed.

Now we use `ffmpeg` to capture actual frames from the camera. This command tells ffmpeg to open `/dev/video0` as a V4L2 source, request 10 fps at 640×480, and save exactly 10 frames as JPEG files.

```bash
ffmpeg -f v4l2 -framerate 10 -video_size 640x480 \
       -i /dev/video0 -frames:v 10 frame_%03d.jpg
```

**Expected output:** Several lines of ffmpeg info followed by `frame=   10 fps=...` and `video:XXXkB`. Check that 10 JPEG files were created:

```bash
ls -la frame_*.jpg
```

**Expected output:** 10 files named `frame_001.jpg` through `frame_010.jpg`, each roughly 20–100 KB depending on scene complexity.

---

### Day 17: OpenCV Frame Capture in C

**Practical Exercise — Timed Frame Grab**

Now we move from command-line tools to programmatic camera access using OpenCV in C. This program opens the camera, sets a low resolution (320×240 for speed), captures 10 frames, and measures how long each frame capture takes. Knowing the capture latency is critical for timing analysis — it becomes the WCET of your camera service.

- `cvCaptureFromCAM(0)`: opens the first available camera (index 0)
- We use 320×240 instead of 640×480 to reduce capture and processing time — important when targeting 10 Hz
- `clock_gettime` before and after `cvQueryFrame` measures the actual time the driver takes to deliver a frame

```c
// file: cam_capture.c
#include <stdio.h>
#include <time.h>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

int main() {
    CvCapture *cap = cvCaptureFromCAM(0);
    if (!cap) { printf("Cannot open camera\n"); return 1; }

    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, 320);
    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, 240);

    for (int i = 0; i < 10; i++) {
        struct timespec t1, t2;
        clock_gettime(CLOCK_MONOTONIC, &t1);

        IplImage *frame = cvQueryFrame(cap);

        clock_gettime(CLOCK_MONOTONIC, &t2);
        long ms = (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_nsec-t1.tv_nsec)/1000000;
        printf("Frame %d: %dx%d captured in %ld ms\n",
               i, frame->width, frame->height, ms);

        char fname[64];
        sprintf(fname, "capture_%02d.jpg", i);
        cvSaveImage(fname, frame, 0);
    }
    cvReleaseCapture(&cap);
    return 0;
}
```

`pkg-config --cflags --libs opencv4` automatically provides the correct compiler flags and library paths for OpenCV. The `-lrt` flag links the real-time library for `clock_gettime`.

```bash
gcc cam_capture.c -o cam_capture $(pkg-config --cflags --libs opencv4) -lrt
./cam_capture
```

**Expected output:**
```
Frame 0: 320x240 captured in 85 ms
Frame 1: 320x240 captured in 33 ms
Frame 2: 320x240 captured in 33 ms
Frame 3: 320x240 captured in 34 ms
Frame 4: 320x240 captured in 33 ms
Frame 5: 320x240 captured in 33 ms
Frame 6: 320x240 captured in 34 ms
Frame 7: 320x240 captured in 33 ms
Frame 8: 320x240 captured in 33 ms
Frame 9: 320x240 captured in 34 ms
```
**Key things to verify:**
- Frame 0 is typically slower (50–200 ms) due to camera initialisation and auto-exposure settling
- Subsequent frames should be ~33 ms (matching 30 fps) or ~100 ms (matching 10 fps) depending on your camera's native frame rate
- The resolution should match 320×240
- If you see `Cannot open camera`, check that no other process has `/dev/video0` open
- 10 JPEG files (`capture_00.jpg` to `capture_09.jpg`) should exist after the run

---

### Day 18: Real-Time Frame Capture Service

**Practical Exercise** — Combine Day 15 sequencer with Day 17 camera capture. Make camera capture a periodic RT service running at 1 Hz under SCHED_FIFO. Measure and log jitter for each frame.

**Key design:**
- Sequencer at 1 Hz releases camera service via semaphore
- Camera service grabs frame, saves with timestamp
- Log capture latency to a CSV file
- Run for 60 seconds, analyze jitter

This is your first self-directed integration exercise. Use the sequencer from Day 15 as a template, replace the `printf` in Service 1 with the OpenCV capture code from Day 17, and add timing instrumentation.

**Expected outcome:** 60 JPEG files named with timestamps, and a `timing.csv` file with one row per frame showing: frame number, expected capture time, actual capture time, jitter (µs). Average jitter should be under 1 ms at 1 Hz.

---

### Day 19: Image Processing — Frame Differencing

**Concepts**
- Simple motion detection via frame differencing
- Grayscale conversion, thresholding
- This is the basis of the "synchronome" project

**Practical Exercise — Motion Detector**

Frame differencing is the simplest form of motion detection: subtract the current frame from the previous frame, and any pixels that changed significantly indicate motion. We prototype this in Python first because it is faster to iterate, then port the critical path to C for performance.

We use `cat << 'EOF' > motion.py` to write the entire script to a file in one command — this is called a "heredoc" and avoids needing a text editor.

```bash
cat << 'EOF' > motion.py
import cv2, time
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

ret, prev = cap.read()
prev_gray = cv2.cvtColor(prev, cv2.COLOR_BGR2GRAY)

for i in range(100):
    t1 = time.monotonic()
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    diff = cv2.absdiff(prev_gray, gray)
    _, thresh = cv2.threshold(diff, 30, 255, cv2.THRESH_BINARY)
    motion = cv2.countNonZero(thresh)
    t2 = time.monotonic()
    print(f"Frame {i}: motion={motion:6d} pixels, time={1000*(t2-t1):.1f}ms")
    prev_gray = gray

cap.release()
EOF
```

Run the motion detector. Wave your hand in front of the camera — you should see the motion pixel count spike. When the scene is still, the count should be near zero (just sensor noise).

```bash
python3 motion.py
```

**Expected output (scene mostly still, with hand wave around frame 30):**
```
Frame 0: motion=   234 pixels, time=35.2ms
Frame 1: motion=   189 pixels, time=34.8ms
Frame 2: motion=   156 pixels, time=33.9ms
...
Frame 29: motion=   120 pixels, time=34.1ms
Frame 30: motion= 18432 pixels, time=35.5ms   ← hand wave!
Frame 31: motion= 22105 pixels, time=34.2ms
Frame 32: motion= 15678 pixels, time=34.8ms
Frame 33: motion=  3421 pixels, time=33.9ms
Frame 34: motion=   198 pixels, time=34.1ms   ← scene still again
...
Frame 99: motion=   145 pixels, time=34.0ms
```
**Key things to verify:**
- Still-scene motion should be low (50–500 pixels — sensor noise and lighting flicker)
- Motion spikes to thousands/tens of thousands when you wave your hand
- Processing time per frame should be ~33–40 ms (dominated by camera capture, not computation)
- If you get `error: (-215:Assertion failed)` the camera failed to open — check `/dev/video0`

---

### Day 20: Timing Analysis & Worst-Case Execution Time

**Concepts**
- WCET measurement: run service many times, record max
- Timing analysis table: for each service, measure min/avg/max execution time
- Relate measured WCET back to RMA utilization calculation

**Practical Exercise**
1. Take your sequencer from Day 15 with camera service from Day 18
2. Run 1000 iterations, log execution time per service to a file
3. Compute min, avg, max, standard deviation
4. Fill in an RMA table and verify schedulability

This `awk` one-liner processes your timing log file (which should have one execution time value per line in milliseconds). `$1` refers to the first column of each line, and the `END` block runs after all lines have been processed.

```bash
awk '{sum+=$1; if($1>max)max=$1; n++} END{print "Avg:",sum/n,"Max:",max,"N:",n}' timing.log
```

**Expected output:**
```
Avg: 34.2 Max: 87.5 N: 1000
```
Your numbers will differ. The Max value is your measured WCET — use this in the RMA utilization formula: U = WCET / Period.

---

## Week 5 — Integration, Test & Verification

### Day 21: The Synchronome Concept

**Concepts**
- Synchronome: use a camera to detect a periodic visual event (e.g., blinking LED) and measure its frequency
- This is the capstone project from the CU course series
- Goal: detect 1 Hz events (easy), then 10 Hz events (hard)

**Practical Exercise — Build a Simple Synchronome**
1. Set up an LED blinking at 1 Hz on the Pi's GPIO (or use a phone screen flashing)
2. Capture frames at 10 Hz using your RT sequencer
3. Detect brightness changes via frame differencing
4. Log timestamps of detected events
5. Calculate measured frequency vs. expected frequency

**Expected outcome:** A log file showing detected event timestamps approximately 1000 ms apart (±10 ms jitter). The measured frequency should be 1.000 Hz ± 0.01 Hz.

---

### Day 22: Deadline Monitoring & Overrun Handling

**Concepts**
- Detecting missed deadlines at runtime
- Graceful degradation strategies
- Watchdog timers

**Practical Exercise**

After each service completes its work, we compare the actual completion time to the expected deadline. If the service took longer than allowed, we log the overrun and increment a counter.

Add this snippet inside each service function in your sequencer, right after the work is done but before the sleep:

```c
clock_gettime(CLOCK_MONOTONIC, &actual);
long overrun_us = ((actual.tv_sec - expected.tv_sec) * 1000000L +
                   (actual.tv_nsec - expected.tv_nsec) / 1000);
if (overrun_us > DEADLINE_US) {
    printf("DEADLINE MISS: S%d overran by %ld us\n", id, overrun_us);
    missed_deadlines++;
}
```

**Expected output (normal operation — no overruns):** No `DEADLINE MISS` lines should appear if your services complete within their period. If you artificially increase the workload (e.g., add a `busy_ms(15)` inside a 10 ms service), you should see:
```
DEADLINE MISS: S1 overran by 5230 us
DEADLINE MISS: S1 overran by 5180 us
```

---

### Day 23: Test & Verification Methods

**Concepts**
- Stress testing: run under heavy CPU/IO load
- Soak testing: run for extended periods (hours)
- Statistical analysis of timing data
- `trace-cmd` and `ftrace` for kernel-level RT debugging

**Practical Exercise**

`trace-cmd` is a front-end for the kernel's built-in `ftrace` tracing system. It lets you record exactly when the kernel switches between threads.

Install the tracing tools:

```bash
sudo apt install trace-cmd
```

**Expected output:** Normal package installation messages.

Start recording scheduling events in the background, run your sequencer, then stop and view the trace.

```bash
sudo trace-cmd record -e sched_switch -e sched_wakeup &
sudo ./sequencer
sudo trace-cmd stop
```

View the first 50 lines of the trace report.

```bash
trace-cmd report | head -50
```

**Expected output:**
```
         sequencer-1234  [000] 1830.100123: sched_switch: prev_comm=sequencer prev_pid=1234 ... ==> next_comm=service next_pid=1235
         service-1235    [000] 1830.100156: sched_switch: prev_comm=service prev_pid=1235 ... ==> next_comm=sequencer next_pid=1234
```
Each line shows a context switch: which thread was running, which thread replaced it, on which CPU, and at what timestamp. Look for your service threads and verify they are being scheduled at the expected times.

Now stress-test your synchronome under heavy load.

```bash
stress --cpu 4 --io 2 &
sudo ./synchronome
kill %1
```

**Expected outcome:** Compare jitter with and without the stress load. SCHED_FIFO services should show minimal jitter increase (< 2× the no-load jitter).

---

### Day 24: Multi-Core RMA Considerations

**Concepts**
- Utilization bound changes with multi-core
- Partitioned scheduling: assign tasks to specific cores, analyze per-core
- Global scheduling: tasks migrate between cores — harder to analyze
- Pi 3B: 4 cores allows isolating RT services from Linux housekeeping

**Practical Exercise**

Linux runs many background tasks that can interfere with RT services. The `isolcpus` kernel parameter tells Linux to *never* schedule normal processes on the specified core(s).

Edit the kernel command line to isolate core 3.

```bash
sudo nano /boot/cmdline.txt
```

Add `isolcpus=3` to the end of the existing line (do not create a new line), save, and reboot.

```bash
sudo reboot
```

After rebooting, pin your critical real-time application to the isolated core.

```bash
sudo taskset -c 3 ./synchronome
```

**Expected outcome:** Jitter should be measurably lower and more consistent compared to running on a non-isolated core. Typical improvement: max jitter drops by 30–60%.

---

### Day 25: Final Integration — Complete RT System

**Capstone Exercise**

Build a complete system that:
1. Uses the sequencer pattern (Day 15)
2. Service 1 (10 Hz): Capture camera frame
3. Service 2 (10 Hz): Frame differencing / motion detection
4. Service 3 (1 Hz): Log results + timing analysis to CSV
5. All services run under SCHED_FIFO with RMA priorities
6. Pinned to specific cores
7. Includes deadline monitoring
8. Run for 60 seconds, produce a timing report

**Deliverables:**
- Source code
- RMA analysis table (hand-calculated)
- Timing report: min/avg/max/jitter per service
- Comparison: with vs. without CPU load
- 1-page writeup: What did you learn about real-time guarantees on Linux?

**Expected outcome:** A working system that captures 600 frames (10 Hz × 60 sec), processes all of them with frame differencing, logs 60 summary entries (1 Hz), and reports zero deadline misses under no-load conditions. Under stress load, fewer than 5% of frames should show deadline misses.

---

## Quick Reference Card

| Topic | Key API / Command |
|---|---|
| Set FIFO scheduling | `sched_setscheduler(0, SCHED_FIFO, &param)` |
| Thread FIFO | `pthread_attr_setschedpolicy(&attr, SCHED_FIFO)` |
| Precise sleep | `clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL)` |
| Measure time | `clock_gettime(CLOCK_MONOTONIC, &ts)` |
| CPU pinning | `taskset -c 0 ./program` or `pthread_attr_setaffinity_np()` |
| Priority inheritance | `pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT)` |
| Camera capture | `v4l2-ctl`, OpenCV `cvCaptureFromCAM()` |
| RMA LUB (n tasks) | `n * (2^(1/n) - 1)` |
| Kernel tracing | `trace-cmd record -e sched_switch` |

---

## Recommended Free Resources

1. **Sam Siewert's GitHub** — `github.com/siewert` — course code examples
2. **POSIX man pages** — `man 7 sched`, `man pthread_create`, `man clock_nanosleep`
3. **"Rate Monotonic Analysis"** — SEI/CMU technical report (free PDF)
4. **Linux RT wiki** — `wiki.linuxfoundation.org/realtime`
5. **V4L2 documentation** — `kernel.org/doc/html/latest/userspace-api/media/v4l/v4l2.html`
