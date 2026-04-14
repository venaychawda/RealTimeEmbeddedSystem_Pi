/* sequencer.c — Day 15: Sequencer Pattern
 *
 * WHY: The backbone RT architecture for the rest of the course. A high-priority
 * sequencer (prio 99) wakes every 10ms and releases services via semaphores.
 * S1: every frame (100 Hz), S2: every 2nd (50 Hz), S3: every 5th (20 Hz).
 * Services run at RMA-assigned priorities.
 *
 * BUILD:  gcc sequencer.c -o sequencer -lpthread -lrt
 * RUN:    sudo taskset -c 0 ./sequencer | head -40
 * EXPECT: S1 every frame, S2 every 2nd, S3 every 5th. In same-frame releases,
 *         S1 prints first (highest prio), then S2, then S3.
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>

#define SEQ_PERIOD_MS 10
#define NUM_FRAMES    100

sem_t sem_s1, sem_s2, sem_s3;
volatile int abort_test = 0;

void *service(void *arg) {
    int id = *(int*)arg;
    sem_t *sem = (id==1) ? &sem_s1 : (id==2) ? &sem_s2 : &sem_s3;
    while (!abort_test) {
        sem_wait(sem);
        if (abort_test) break;
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        printf("S%d @ %ld.%03ld\n", id, ts.tv_sec%100, ts.tv_nsec/1000000);
    }
    return NULL;
}

int main() {
    struct sched_param p = { .sched_priority = 99 };
    sched_setscheduler(0, SCHED_FIFO, &p);

    sem_init(&sem_s1, 0, 0);
    sem_init(&sem_s2, 0, 0);
    sem_init(&sem_s3, 0, 0);

    int ids[] = {1, 2, 3};
    int prios[] = {90, 80, 70};
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
        sem_post(&sem_s1);
        if (frame % 2 == 0) sem_post(&sem_s2);
        if (frame % 5 == 0) sem_post(&sem_s3);

        next.tv_nsec += SEQ_PERIOD_MS * 1000000L;
        if (next.tv_nsec >= 1000000000L) { next.tv_sec++; next.tv_nsec -= 1000000000L; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }

    abort_test = 1;
    sem_post(&sem_s1); sem_post(&sem_s2); sem_post(&sem_s3);
    for (int i = 0; i < 3; i++) pthread_join(threads[i], NULL);
    return 0;
}
