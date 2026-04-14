/* test_rt.c — Day 3: Verify POSIX RT scheduling API availability
 *
 * WHY: This program queries the kernel for the range of priority levels
 * available under SCHED_FIFO. If the kernel supports POSIX real-time,
 * it will report a range of 1–99. This confirms your toolchain and
 * kernel are ready for all RT programming in this course.
 *
 * BUILD:  gcc test_rt.c -o test_rt -lpthread
 * RUN:    sudo ./test_rt
 * EXPECT: SCHED_FIFO priority range: 1 - 99
 */

#include <stdio.h>
#include <sched.h>
#include <pthread.h>

int main() {
    int policy_max = sched_get_priority_max(SCHED_FIFO);
    int policy_min = sched_get_priority_min(SCHED_FIFO);
    printf("SCHED_FIFO priority range: %d - %d\n", policy_min, policy_max);

    if (policy_min == 1 && policy_max == 99) {
        printf("  ✓ POSIX RT scheduling is available. You have 99 RT priority levels.\n");
    } else if (policy_min == -1) {
        printf("  ✗ SCHED_FIFO not supported by this kernel.\n");
    }
    return 0;
}
