/* posix_timer.c — Day 14: POSIX Timer-Driven Periodic Service
 *
 * WHY: Instead of clock_nanosleep loops, the kernel can push signals at
 * regular intervals. This timer fires SIGALRM every 10ms. Run for 1 second
 * and expect ~100 ticks.
 *
 * NOTES:
 * - We use sigaction() instead of signal(). signal() may reset the handler
 *   back to SIG_DFL after the first invocation on some systems, causing
 *   only 1 tick. sigaction() keeps the handler registered permanently.
 * - We use write() instead of printf() inside the handler because printf
 *   is NOT async-signal-safe — it can deadlock if the main thread holds
 *   stdout's internal lock when the signal arrives.
 *
 * BUILD:  gcc posix_timer.c -o posix_timer -lrt
 * RUN:    sudo ./posix_timer
 * EXPECT: ~100 ticks, timestamps ~10ms apart. "Total ticks: 100"
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

volatile int count = 0;

void timer_handler(int sig) {
    /* Only increment the counter inside the handler.
     * write() is async-signal-safe; printf is NOT.
     * We format a short message with write() for per-tick visibility. */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    char buf[80];
    int len = snprintf(buf, sizeof(buf), "Tick %d @ %ld.%06ld\n",
                       count++,
                       ts.tv_sec % 100, ts.tv_nsec / 1000);
    write(STDOUT_FILENO, buf, len);
}

int main() {
    struct sched_param p = { .sched_priority = 80 };
    sched_setscheduler(0, SCHED_FIFO, &p);

    /* Use sigaction instead of signal — handler stays registered permanently.
     * SA_RESTART: automatically restart interrupted system calls (like sleep).
     * signal() may reset the handler to SIG_DFL after one invocation. */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = timer_handler;
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    timer_t tid;
    struct sigevent sev = { .sigev_notify = SIGEV_SIGNAL, .sigev_signo = SIGALRM };
    timer_create(CLOCK_MONOTONIC, &sev, &tid);

    struct itimerspec its = {
        .it_interval = { .tv_sec = 0, .tv_nsec = 10000000 }, /* 10 ms repeat */
        .it_value    = { .tv_sec = 0, .tv_nsec = 10000000 }  /* 10 ms initial */
    };
    timer_settime(tid, 0, &its, NULL);

    struct timespec wait = { .tv_sec = 1, .tv_nsec = 0 };
    while (nanosleep(&wait, &wait) == -1);
    printf("Total ticks: %d\n", count);
    return 0;
}
