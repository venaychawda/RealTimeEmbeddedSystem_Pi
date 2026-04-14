/* priority_inversion.c — Day 11: Priority Inversion Demo
 *
 * WHY: A high-priority thread blocked by a low-priority mutex holder is
 * a priority inversion. PTHREAD_PRIO_INHERIT fixes this by temporarily
 * boosting the blocker's priority. Mars Pathfinder crashed from this in 1997.
 *
 * STEP 1: Run as-is — observe HIGH blocked ~150ms.
 * STEP 2: Uncomment PTHREAD_PRIO_INHERIT line, rebuild. Same wait in this
 *         simple case, but adding a medium-priority thread would show the fix.
 *
 * BUILD:  gcc priority_inversion.c -o prio_inv -lpthread -lrt
 * RUN:    sudo taskset -c 0 ./prio_inv
 */

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t shared_resource;

void busy_ms(int ms) {
    struct timespec s, n;
    clock_gettime(CLOCK_MONOTONIC, &s);
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &n);
        if ((n.tv_sec-s.tv_sec)*1000 + (n.tv_nsec-s.tv_nsec)/1000000 >= ms) break;
    }
}

void *low_prio(void *arg) {
    printf("LOW: locking mutex\n");
    pthread_mutex_lock(&shared_resource);
    printf("LOW: got mutex, working 200ms...\n");
    busy_ms(200);
    printf("LOW: releasing mutex\n");
    pthread_mutex_unlock(&shared_resource);
    return NULL;
}

void *high_prio(void *arg) {
    usleep(50000); /* start 50ms late so LOW grabs mutex first */
    printf("HIGH: trying to lock mutex...\n");
    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pthread_mutex_lock(&shared_resource);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    long wait_ms = (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_nsec-t1.tv_nsec)/1000000;
    printf("HIGH: got mutex after %ld ms (blocked!)\n", wait_ms);
    pthread_mutex_unlock(&shared_resource);
    return NULL;
}

int main() {
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    /* pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT); */ /* uncomment to fix */
    pthread_mutex_init(&shared_resource, &mattr);

    pthread_t tl, th;
    pthread_attr_t attr;
    struct sched_param sp;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    sp.sched_priority = 10;
    pthread_attr_setschedparam(&attr, &sp);
    pthread_create(&tl, &attr, low_prio, NULL);

    sp.sched_priority = 90;
    pthread_attr_setschedparam(&attr, &sp);
    pthread_create(&th, &attr, high_prio, NULL);

    pthread_join(tl, NULL);
    pthread_join(th, NULL);
    return 0;
}
