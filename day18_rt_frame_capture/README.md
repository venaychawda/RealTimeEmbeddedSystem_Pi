# Day 18: Real-Time Frame Capture Service

## Goal
Combine the sequencer (Day 15) with OpenCV capture (Day 17).

## Self-Directed Exercise
1. Copy `sequencer.c` from Day 15 as your starting point
2. Replace `printf` in Service 1 with OpenCV frame capture from Day 17
3. Set sequencer to 1 Hz (change `SEQ_PERIOD_MS` to 1000)
4. Add timing instrumentation: log capture latency to `timing.csv`
5. Run for 60 seconds, analyze jitter

## Expected Outcome
- 60 JPEG files with timestamps
- `timing.csv` with columns: frame_number, expected_time, actual_time, jitter_us
- Average jitter < 1 ms at 1 Hz
