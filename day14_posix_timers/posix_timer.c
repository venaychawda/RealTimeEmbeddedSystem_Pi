/* posix_timer.c — Day 14: POSIX Timer-Driven Periodic Service
 *
 * WHY: Instead of clock_nanosleep loops, the kernel can push signals at
 * regular intervals. This timer fires SIGALRM every 10ms. Run for 1 second
 * and expect ~100 ticks.
 *
 * BUILD:  gcc posix_timer.c -o posix_timer -lrt
 * RUN:    sudo ./posix_timer
 * EXPECT: ~100 ticks, timestamps ~10ms apart. "Total ticks: 100"
 */

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
    struct sched_param p = { .sched_priority = 80 };
    sched_setscheduler(0, SCHED_FIFO, &p);

    signal(SIGALRM, timer_handler);

    timer_t tid;
    struct sigevent sev = { .sigev_notify = SIGEV_SIGNAL, .sigev_signo = SIGALRM };
    timer_create(CLOCK_MONOTONIC, &sev, &tid);

    struct itimerspec its = {
        .it_interval = { .tv_sec = 0, .tv_nsec = 10000000 },
        .it_value    = { .tv_sec = 0, .tv_nsec = 10000000 }
    };
    timer_settime(tid, 0, &its, NULL);

    sleep(1);
    printf("Total ticks: %d\n", count);
    return 0;
}
