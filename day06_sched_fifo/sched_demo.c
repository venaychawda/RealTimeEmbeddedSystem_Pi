/* sched_demo.c — Day 6: SCHED_FIFO Priority Preemptive Scheduling
 *
 * WHY: Demonstrates that under SCHED_FIFO, the highest-priority runnable
 * thread ALWAYS runs first. On a single core, threads run in strict
 * priority order: 90, then 50, then 10.
 *
 * CRITICAL: We use a semaphore "starting gate" so all threads are created
 * and blocked BEFORE any begin working. Without this, each thread would
 * preempt main() upon creation (any SCHED_FIFO > SCHED_OTHER) and run
 * to completion before the next thread even exists — giving the misleading
 * result of Thread 0 (lowest priority) finishing first.
 *
 * BUILD:  gcc sched_demo.c -o sched_demo -lpthread -lrt
 * RUN:    sudo taskset -c 0 ./sched_demo
 * EXPECT: Thread 2 (prio 90) finishes first, then Thread 1 (50), then Thread 0 (10).
 *         Each thread's start time ≈ previous thread's end time (serial on 1 core).
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>

sem_t start_gate;

void *worker(void *arg) {
    int id = *(int *)arg;

    /* Block until main() releases all threads simultaneously */
    sem_wait(&start_gate);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

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
    int prios[] = {10, 50, 90};

    /* Step 1: Create all threads — they immediately block on sem_wait */
    for (int i = 0; i < 3; i++) {
        pthread_attr_init(&attr);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        param.sched_priority = prios[i];
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&t[i], &attr, worker, &ids[i]);
    }

    /* Step 2: Release all threads simultaneously */
    for (int i = 0; i < 3; i++)
        sem_post(&start_gate);

    for (int i = 0; i < 3; i++) pthread_join(t[i], NULL);
    sem_destroy(&start_gate);
    return 0;
}
