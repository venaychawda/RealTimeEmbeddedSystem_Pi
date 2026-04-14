# Day 25: Capstone — Complete Real-Time System

## Specification
Build a system with:
1. Sequencer pattern (Day 15)
2. Service 1 (10 Hz): Camera frame capture
3. Service 2 (10 Hz): Frame differencing / motion detection
4. Service 3 (1 Hz): Log results + timing to CSV
5. All under SCHED_FIFO with RMA priorities
6. Pinned to specific cores
7. Deadline monitoring
8. Run 60 seconds, produce timing report

## Deliverables
- [ ] Source code (single `.c` file or multi-file with Makefile)
- [ ] RMA analysis table (hand-calculated)
- [ ] Timing report: min/avg/max/jitter per service
- [ ] Comparison: with vs. without CPU stress load
- [ ] 1-page writeup: What did you learn about RT guarantees on Linux?

## Success Criteria
- 600 frames captured (10 Hz × 60 sec)
- All frames processed with motion detection
- 60 log entries (1 Hz)
- **Zero** deadline misses under no-load
- **< 5%** deadline misses under `stress --cpu 4`
