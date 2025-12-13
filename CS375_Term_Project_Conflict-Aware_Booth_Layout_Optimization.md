# Conflict-Aware Booth

Layout Optimization By Vishil Patel and Alex Eskenazi

## Page 1

Conflict-Aware Booth
Layout Optimization
By Vishil Patel and Alex Eskenazi

![Page 1](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_01.png)

## Page 2

Business Problem: Optimize Booth Layout

![Page 2](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_02.png)

## Page 3

Conflict-Aware Booth Layout Optimization
The Problem:
-
Based on Event Floor Planning Internship - manually arranging vendor booths is very
time consuming
-
Goal: Automatically place booths to maximize exposure while avoiding competitor
conflicts
Input:
-
Grid of slots with position bonuses (entrance/aisle = higher value)
-
Booths with category and value
-
Some slots blocked (info desk, storage)
Output: Optimal booth assignments

![Page 3](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_03.png)

## Page 4

Optimization Objectives
Score Formula:
Score = Exposure - 0.6×RowClashes - 0.3×AcrossAisleClashes
Three Components:
1.
Exposure = Σ(booth value × position bonus)
  -
Put high value booths in prime spots
2.
Row Clashes = Same-category booths next to each other
  -
Penalty for horizontal neighbors
3.
Across-Aisle Clashes = Same-category booths facing each other
  -
Penalty for vertical neighbors
Why Hard? NP-Hard problem
-
20 booths in 30 slots = 30! / (30-10)! ~= 10²⁶ possibilities!

![Page 4](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_04.png)

## Page 5

Our Two-Stage Algorithm
Stage 1: Greedy Construction O(n² × m)
1.
Sort booths by value O(n log n)
2.
For each booth: (n)
  -
Try all empty slots (m)
  -
Pick slot with best score contribution (booth x location x constraints)
3.
Produces good initial solution fast
Stage 2: Local Search O(k × n²)
-
Two move types:
  -
Swap: Exchange two booth positions
  -
Relocate: Move booth to empty slot
-
Accept only improvements
-
Stop after 1000 non-improving iterations (early termination)
Key Optimizations:
-
Only swap nearby booths (same/adjacent rows)
-
Sample 30 random empty slots (not all)
-
Early termination saves time

![Page 5](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_05.png)

## Page 6

Greedy Placement

![Page 6](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_06.png)

## Page 7

Local Search

![Page 7](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_07.png)

## Page 8

Example
Score: 220
Score: 202
Random Layout
Greedy Layout
Empty Layout
- Orange number (top-right corner) = Position bonus (0.3-0.9, higher is better location)
- Center number (e.g., #5) = Booth ID
- Category name = Vendor category (Food, Tech, Apparel, etc.)
- v=number = Booth value

![Page 8](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_08.png)

## Page 9

Results & Performance (so far)
Test Instance 1: 4×8 grid, 10 booths
-
Greedy: 78.9
-
Final: 78.9 (no improvement needed - already optimal!)
-
Converged in ~20 iterations
Test Instance 2: 6×6 grid, 15 booths
-
Greedy: 163.0
-
Final: 163.6 (+0.6)
-
Converged in ~150 iterations
Comparison to Baselines:
-
Random (best of 10): 68.6
-
Our algorithm: 78.9
-
Improvement: +15% better
Performance:
-
Runtime: < 0.2 seconds for 15 booths
-
Scales to 50-100 booths (typical event size)
-
Space: O(n + m) - very efficient
Greedy Local Search
     Time
Complexity

![Page 9](CS375_Term_Project_Conflict_Aware_Booth_Layout_Optimization_assets/page_09.png)
