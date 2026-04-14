# Real-Time Embedded Systems Training — Top-Level Makefile
# Build all exercises, or a single day: make day06

DAYS_WITH_MAKEFILES = day03_pi_setup day04_pthreads_basics day05_mutex_sync \
	day06_sched_fifo day07_rt_clock_jitter day08_cyclic_executive \
	day10_rms_implementation day11_priority_inversion day12_multicore_affinity \
	day13_semaphores_prodcon day14_posix_timers day15_sequencer_pattern \
	day17_opencv_capture

.PHONY: all clean $(DAYS_WITH_MAKEFILES)

all: $(DAYS_WITH_MAKEFILES)

$(DAYS_WITH_MAKEFILES):
	@echo "=== Building $@ ==="
	$(MAKE) -C $@

clean:
	@for d in $(DAYS_WITH_MAKEFILES); do \
		echo "=== Cleaning $$d ==="; \
		$(MAKE) -C $$d clean; \
	done

# Convenience targets: make day06
day03: day03_pi_setup
day04: day04_pthreads_basics
day05: day05_mutex_sync
day06: day06_sched_fifo
day07: day07_rt_clock_jitter
day08: day08_cyclic_executive
day10: day10_rms_implementation
day11: day11_priority_inversion
day12: day12_multicore_affinity
day13: day13_semaphores_prodcon
day14: day14_posix_timers
day15: day15_sequencer_pattern
day17: day17_opencv_capture
