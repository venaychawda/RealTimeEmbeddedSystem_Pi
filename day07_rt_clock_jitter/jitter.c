/* jitter.c — Day 7: POSIX Real-Time Clock & Jitter Measurement
 *
 * WHY: A real-time service must wake up at precise intervals. "Jitter" is
 * the difference between when it SHOULD wake up and when it ACTUALLY does.
 * This program measures wake-up jitter at 100 Hz (10 ms period) using
 * SCHED_FIFO and CLOCK_MONOTONIC with absolute-time sleeping.
 *
 * BUILD:  gcc jitter.c -o jitter -lrt
 * RUN:    sudo ./jitter
 * EXPECT: Avg: 10-50 us, Max: 50-200 us (on Pi 3B, no load)
 */

#include <stdio.h>
#include <time.h>
#include <sched.h>

#define ITERATIONS 100
#define PERIOD_MS 10  /* 10 ms = 100 Hz */

int main() {
    struct sched_param param = { .sched_priority = 80 };
    sched_setscheduler(0, SCHED_FIFO, &param);

    struct timespec next, now;
    double jitters[ITERATIONS];
    clock_gettime(CLOCK_MONOTONIC, &next);

    for (int i = 0; i < ITERATIONS; i++) {
        next.tv_nsec += PERIOD_MS * 1000000L;
        if (next.tv_nsec >= 1000000000L) {
            next.tv_sec++;
            next.tv_nsec -= 1000000000L;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        clock_gettime(CLOCK_MONOTONIC, &now);

        long diff_ns = (now.tv_sec - next.tv_sec) * 1000000000L
                     + (now.tv_nsec - next.tv_nsec);
        jitters[i] = diff_ns / 1000.0;
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
