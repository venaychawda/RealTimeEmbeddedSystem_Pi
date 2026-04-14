/* pthread_hello.c — Day 4: POSIX Threads Basics
 *
 * WHY: Real-time services run as threads. Before adding RT scheduling,
 * we learn thread creation, argument passing, and joining. The key
 * observation: output interleaves unpredictably under default (fair)
 * scheduling. This non-determinism is what RT scheduling fixes.
 *
 * BUILD:  gcc pthread_hello.c -o pthread_hello -lpthread
 * RUN:    ./pthread_hello
 * EXPECT: Thread outputs interleaved in varying order each run.
 *         "All threads done." always appears last.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3

void *thread_func(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 5; i++) {
        printf("Thread %d: iteration %d\n", id, i);
        usleep(100000); /* 100 ms — makes interleaving visible */
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
