# Day 9: Rate Monotonic Scheduling — Theory

## Goal
Learn RMA hand-calculation: utilization, LUB, and schedulability test.

## Key Formula
**LUB for n tasks = n × (2^(1/n) − 1)**

| n tasks | LUB    |
|---------|--------|
| 1       | 1.000  |
| 2       | 0.828  |
| 3       | 0.780  |
| 4       | 0.757  |
| ∞       | 0.693  |

## Exercise — Hand Calculation
```
Task    Period (ms)    WCET (ms)    Utilization
T1      10             2            0.200
T2      20             4            0.200
T3      50             10           0.200
                            Total:  0.600

LUB for 3 tasks = 0.780
0.600 < 0.780 → SCHEDULABLE ✓
```

## Problems (with answers)

**Q1:** Max WCET for T3 before unschedulable?
**A1:** T1+T2 = 0.400. Remaining = 0.780 − 0.400 = 0.380. WCET_max = 0.380 × 50 = **19 ms**.

**Q2:** Add T4 (period 100ms). New LUB? Max T4 WCET?
**A2:** LUB(4) = 0.757. T1+T2+T3 = 0.600. Remaining = 0.157. WCET_max = 0.157 × 100 = **15.7 ms**.

**Q3:** Is utilization above LUB but below 1.0 necessarily unschedulable?
**A3:** **No.** LUB is a sufficient condition, not necessary. Use response-time analysis for exact test.
