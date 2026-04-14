# Day 20: Timing Analysis & Worst-Case Execution Time

## Goal
Measure min/avg/max execution time per service and build an RMA table.

## Self-Directed Exercise
1. Use your Day 18 sequencer with camera service
2. Log execution time per iteration to `timing.log`
3. Run 1000 iterations
4. Analyze with: `awk '{sum+=$1; if($1>max)max=$1; n++} END{print "Avg:",sum/n,"Max:",max,"N:",n}' timing.log`
5. Fill in RMA table: U = WCET_measured / Period. Check against LUB.

## Expected Output
```
Avg: 34.2 Max: 87.5 N: 1000
```
The Max is your measured WCET.
