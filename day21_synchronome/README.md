# Day 21: The Synchronome

## Goal
Detect a periodic visual event (blinking LED) and measure its frequency.

## Exercise
1. Set up an LED blinking at 1 Hz (Pi GPIO or phone screen)
2. Capture frames at 10 Hz using your RT sequencer
3. Detect brightness changes via frame differencing
4. Log timestamps of detected events
5. Calculate measured frequency vs. expected

## Expected Outcome
Event timestamps ~1000 ms apart (±10 ms jitter). Measured frequency: 1.000 ± 0.01 Hz.
