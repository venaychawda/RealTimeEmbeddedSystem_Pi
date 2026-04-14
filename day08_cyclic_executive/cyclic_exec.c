/* cyclic_exec.c — Day 8: Cyclic Executive
 *
 * WHY: The simplest RT architecture — a timed loop calling services in
 * a fixed sequence. No threads, no OS scheduler, pure determinism.
 * Major frame = 30 iterations, minor frame = 10 ms.
 * S1 every frame, S2 every 2nd, S3 every 3rd.
 *
 * BUILD:  gcc cyclic_exec.c -o cyclic_exec -lrt
 * RUN:    sudo ./cyclic_exec
 * EXPECT: S1 every 10ms, S2 every 20ms, S3 every 30ms. Frame 0 has all three.
 */

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
    struct sched_param p = { .sched_priority = 90 };
    sched_setscheduler(0, SCHED_FIFO, &p);

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    for (int frame = 0; frame < 30; frame++) {
        service1(); print_time();
        if (frame % 2 == 0) { service2(); print_time(); }
        if (frame % 3 == 0) { service3(); print_time(); }

        next.tv_nsec += 10000000L;
        if (next.tv_nsec >= 1000000000L) { next.tv_sec++; next.tv_nsec -= 1000000000L; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return 0;
}
