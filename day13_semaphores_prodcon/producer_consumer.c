/* producer_consumer.c — Day 13: Semaphore-based Producer/Consumer
 *
 * WHY: Fundamental RT pattern — one service produces data, another consumes.
 * Two counting semaphores (empty/full) coordinate without busy-waiting.
 * Producer is faster (50ms) than consumer (80ms) — observe buffer filling.
 *
 * BUILD:  gcc producer_consumer.c -o prodcon -lpthread
 * RUN:    ./prodcon
 * EXPECT: Items 0-19 produced and consumed in FIFO order.
 */

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
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in_idx] = i;
        printf("Produced: %d at slot %d\n", i, in_idx);
        in_idx = (in_idx + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        usleep(50000);
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int val = buffer[out_idx];
        printf("  Consumed: %d from slot %d\n", val, out_idx);
        out_idx = (out_idx + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        usleep(80000);
    }
    return NULL;
}

int main() {
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_t p, c;
    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    return 0;
}
