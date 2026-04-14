/* multicore_rt.c — Day 12: CPU Affinity & Partitioned Scheduling
 *
 * WHY: Pinning RT tasks to specific cores prevents migration and cache
 * thrashing, making timing analysis tractable. 4 tasks pinned to 4 cores
 * run in parallel (~50ms total); all on core 0 run serially (~200ms).
 *
 * BUILD:  gcc multicore_rt.c -o multicore_rt -lpthread -lrt
 * RUN:    sudo ./multicore_rt
 */

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

        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);   /* pin task i to core i */
        pthread_attr_setaffinity_np(&attr, sizeof(cpuset), &cpuset);

        pthread_create(&t[i], &attr, task, &ids[i]);
    }
    for (int i = 0; i < 4; i++) pthread_join(t[i], NULL);
    return 0;
}
