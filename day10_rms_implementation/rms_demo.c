/* rms_demo.c — Day 10: Rate Monotonic Scheduling Implementation
 *
 * WHY: Implements the RMA task set from Day 9 in real code.
 * T1: 10ms period, 2ms WCET, priority 90 (fastest → highest)
 * T2: 20ms period, 4ms WCET, priority 80
 * T3: 50ms period, 10ms WCET, priority 70
 * Uses semaphore gate (same reason as Day 6).
 *
 * BUILD:  gcc rms_demo.c -o rms_demo -lpthread -lrt
 * RUN:    sudo taskset -c 0 ./rms_demo
 * EXPECT: T1 fires most often, T2 every ~20ms, T3 every ~50ms.
 */

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
    sem_wait(&start_sem);

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    for (int i = 0; i < p->iterations; i++) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        printf("T%d start @ %ld.%03ld\n", p->id,
               ts.tv_sec % 100, ts.tv_nsec / 1000000);

        busy_work_ms(p->wcet_ms);

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

    for (int i = 0; i < n; i++) sem_post(&start_sem);
    for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);
    sem_destroy(&start_sem);
    return 0;
}
