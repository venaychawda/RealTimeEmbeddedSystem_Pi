/* race.c — Day 5: Race Condition & Mutex Fix
 *
 * WHY: counter++ is NOT atomic — it's 3 assembly instructions (load,
 * increment, store). Two threads doing this concurrently lose increments.
 * A mutex serializes access, fixing the race.
 *
 * STEP 1: Compile and run as-is. Counter will be wrong (< 2000000).
 * STEP 2: Uncomment the 3 mutex lines. Recompile. Counter = 2000000.
 *
 * BUILD:  gcc race.c -o race -lpthread
 * RUN:    ./race
 * EXPECT (no mutex):  Counter = 1340287 (expected 2000000) — varies each run
 * EXPECT (with mutex): Counter = 2000000 (expected 2000000) — every time
 */

#include <stdio.h>
#include <pthread.h>

int counter = 0;
/* pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; */  /* uncomment to fix */

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        /* pthread_mutex_lock(&lock);   */  /* uncomment to fix */
        counter++;
        /* pthread_mutex_unlock(&lock); */  /* uncomment to fix */
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter = %d (expected 2000000)\n", counter);
    return 0;
}
