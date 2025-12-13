# Conflict-Aware Booth Layout Optimization

**CS375 - Algorithm Design and Analysis**

**Team Members:** Vishil Patel and Alex Eskenazi

**Date:** December 13, 2025

## 1. Problem Statement

### 1.1 Background and Motivation

The booth layout problem comes from a real experience at an event planning internship where vendor booths had to be manually arranged for trade shows and conventions. For this project, the goal is to automatically assign vendor booths to floor positions in a way that maximizes overall quality while avoiding placing competing vendors (same category) next to each other. This requirement is close to the real world situation, but we have modified it so that the original company's secret sauce is not revealed, but the algorithm would be applicable with minor changes.

This is important because:

* High-value vendors should get premium locations (near entrances, main aisles)
* Competing vendors shouldn't be placed side-by-side or directly across from each other
* Manual layout planning takes hours and often misses better arrangements

### 1.2 Problem Definition

**Input:**

* A rectangular grid of R rows × C columns representing available floor space
* A set of N vendor booths, each with:
  * Category (e.g., Food, Tech, Apparel)
  * Value representing importance/revenue potential
* Position bonuses for each grid slot (higher near entrances/aisles)
* Some slots may be blocked (reserved for info desks, storage, etc.)

**Output:**

* An assignment of booths to slots that maximizes a scoring function

**Scoring Function:**

```
Score = Exposure - (0.6 × RowClashes) - (0.3 × AcrossAisleClashes)
```

Where:

* **Exposure** = Σ(booth value × position bonus) for all placed booths
* **RowClashes** = number of same-category booth pairs next to each other horizontally
* **AcrossAisleClashes** = number of same-category booth pairs facing each other vertically
  * We treat rows r and r+1 as facing across an aisle; slot (r,c) faces (r+1,c) if both exist

The weights (0.6 and 0.3) basically say that side-by-side conflicts are worse than across-aisle conflicts. We picked these values based on what seemed reasonable, in a real system we would probably tune them based on feedback from event organizers.

### 1.3 Problem Complexity

Our scoring uses pairwise penalties for clashes (when same-category booths are neighbors) and bonuses for good positions. This is a combinatorial optimization problem where we have to consider both individual slot quality and interactions between neighboring booths. These kinds of problems tend to be really hard because we can't just optimize each booth independently.

The number of possible assignments also explodes really fast. With n available slots and k booths to place, the number of ways to assign them is:

```
nPk = n! / (n - k)!
```

For n=30, k=20: 30!/(10!) ≈ 7.3×10^25 (about 10^26) possibilities. That's way too many to check, it would take way longer than the universe has existed. So we need a heuristic approach that finds good solutions quickly instead of trying to find the absolute best one.

## 2. Algorithm Description

We ended up using a two-stage approach: first build a decent solution with a greedy algorithm, then improve it with local search.

### 2.1 Stage 1: Greedy Construction

The basic idea is to place booths one at a time, always putting each booth in its best available spot given what's already been placed.

**Algorithm:**

```
1. Sort booths by value (highest first)
   - If values are equal, put rarer categories first

2. For each booth in that order:
   a. Look at all empty slots that aren't blocked
   b. For each slot, calculate how much it would add to the score:
      contribution = (booth_value × slot_bonus)
                   - (0.6 × new_row_clashes)
                   - (0.3 × new_across_clashes)
   c. Place the booth in whichever slot gives the best contribution

3. Done, return the layout
```

**Time complexity:** For each of n booths we evaluate up to m slots. If we use a delta calculation (just check that slot's exposure and its few neighbors), each evaluation is O(1), so greedy is **O(nm)**. In practice we recalculate the full score each time which is slower, but the big-O is still O(nm).

This works because the high-value booths get to pick their spots first, so they grab the premium locations. And since we place booths one at a time, the early ones have fewer neighbors to conflict with. This generally gives us a pretty good starting point.

### 2.2 Stage 2: Local Search

After the greedy placement, we try to make small improvements by swapping booths around or moving them to empty spots.

**Move Types:**





1. **Swap:** Exchange positions of two placed booths
2. **Relocate:** Move a booth to an empty slot

**Algorithm:**

```
Set no_improvement_count = 0
Loop until no_improvement_count >= 1000:
    current_score = calculate_total_score()

    # Try swapping pairs (only nearby booths to save time)
    For each pair of booths in same or adjacent rows:
        Swap them temporarily
        Calculate new score
        If it's better than anything we've seen:
            Remember this swap
        Swap back

    # Try moving booths to empty spots (sample 30 random ones)
    For each placed booth:
        For 30 randomly picked empty slots:
            Move booth there temporarily
            Calculate new score
            If it's better than anything we've seen:
                Remember this move
            Move back

    If we found an improvement:
        Apply it
        Reset no_improvement_count to 0
    Else:
        Increment no_improvement_count

Return the final layout
```

**Key tricks we used:**

* **Neighborhood restriction:** Only try swapping booths that are in the same row or adjacent rows. Swapping booths that are far apart usually doesn't help much anyway and increases the run time.
* **Random sampling:** Instead of trying every single empty slot, just pick 30 at random. This makes it way faster.
* **Early termination:** If we go 1000 iterations without finding any improvement, just stop. Usually we converge much sooner than that anyway.

**Time complexity:** Each iteration tries swapping booths in nearby rows (up to O(n²) pairs) plus about 30 relocate moves per booth (O(n)). With constant-time score checks, each iteration is **O(n²)**, so after k iterations the search is **O(k n²)**. In practice k was 20–200 because of early termination.

### 2.3 Implementation Details

We implemented this in C++ and split it into several files to keep things organized:

* **model.h/cpp:** Basic data structures (Booth, Slot, Instance, Layout)
* **io.h/cpp:** Reading input files
* **score.h/cpp:** All the scoring functions
* **greedy.h/cpp:** The greedy algorithm
* **localsearch.h/cpp:** The local search with swaps and relocations
* **main.cpp:** Puts it all together

**Design choices:**

* Used simple vectors and loops
* Kept all scores as doubles to avoid rounding issues
* Added a random seed that gets printed out so we can reproduce runs if we need to debug
* Made clash counts integers so they're easier to read

## 3. Results and Discussion

### 3.1 Test Instances

We created three test instances to see how well local search works at fixing conflicts:

**Instance 1 (greedy_trap.txt):**

* Grid: 3 rows × 6 columns (18 total slots)
* Booths: 12 booths across 5 categories
* Blocked slots: 2
* Available slots: 16 (67% full)
* Booth values: 15-35
* Designed with overlapping values to trick greedy into creating conflicts

**Instance 2 (almost_full.txt):**

* Grid: 6 rows × 7 columns (42 total slots)
* Booths: 38 booths across 7 categories
* Blocked slots: 3
* Available slots: 39 (97% full)
* Booth values: 30-40
* Almost completely full, so greedy has a hard time avoiding conflicts

**Instance 3 (super_tight.txt):**

* Grid: 7 rows × 8 columns (56 total slots)
* Booths: 50 booths across 9 categories
* Blocked slots: 4
* Available slots: 52 (96% full)
* Booth values: 32-50
* Tons of Food, Tech, and Apparel booths (most booths are in just 3 categories)

### 3.2 Experimental Results

We ran everything with seed 42 so the results are reproducible.

**Instance 1 Results (greedy_trap.txt):**

```
Using seed: 42
Running greedy...
Greedy Score: 142.0
  Row Clashes: 3
  Across Clashes: 2

Running local search...

Final Score: 143.8
  Row Clashes: 0
  Across Clashes: 0

Improvement: +1.8
  Clashes removed: 5

Layout:
  X  2  4  0  3  .
  .  5  1  7  6  .
 11  8 10  9  .  X
```

On this instance the greedy algorithm created 5 total clashes because the similar booth values caused it to place same-category vendors too close together. The local search rearranged things and got rid of all the conflicts, improving the score by 1.8 points.

**Instance 2 Results (almost_full.txt):**

```
Using seed: 42
Running greedy...
Greedy Score: 1000.6
  Row Clashes: 2
  Across Clashes: 1

Running local search...

Final Score: 1002.0
  Row Clashes: 0
  Across Clashes: 0

Improvement: +1.4
  Clashes removed: 3

Layout:
  X 36 21 14 23 12 20
 13 29  5 22 28 27  3
 18  0  8 31 24  1 11
 26  2  X  6 17 35 15
 30 10 33  7 25  4 32
 19 34 37  9 16  .  X
```

With 38 booths in 39 available slots (97% full), the greedy algorithm had trouble avoiding conflicts. The grid is so packed that some same-category booths ended up next to each other. Local search found ways to rearrange them and removed all 3 clashes, improving the score by 1.4 points.

**Instance 3 Results (super_tight.txt):**

```
Using seed: 42
Running greedy...
Greedy Score: 1562.6
  Row Clashes: 1
  Across Clashes: 4

Running local search...

Final Score: 1563.5
  Row Clashes: 0
  Across Clashes: 2

Improvement: +0.9
  Clashes removed: 3

Layout:
  X 45 42  7 39 27 13  X
 48 10 23 34 20  5 28 49
 14 25  3 17 33 21  8 31
 43  6 32  1 18  0 38 11
 12 40  4 19  2 36 26 47
  . 46 24 35 22 37 44  .
  X 16 30  9 41 29 15  X
```

This was the toughest instance with 50 booths in 52 available slots (96% full) and a ton of booths in just 3 categories. The greedy algorithm created 5 clashes, and local search removed 3 of them for a +0.9 improvement. The remaining 2 across-aisle clashes are basically unavoidable since there are so many Food and Tech booths.

### 3.3 Performance Analysis

**Runtime:**

* Instance 1 (12 booths): under 0.1 seconds
* Instance 2 (38 booths): under 0.3 seconds
* Instance 3 (50 booths): under 0.5 seconds
* All tests on a regular laptop

**How good are the solutions?**

The greedy algorithm by itself builds a pretty decent layout, but it sometimes creates conflicts when it focuses too much on getting high-value booths into premium spots. The local search is really valuable for fixing these problems:

* **Instance 1:** Greedy created 5 clashes, local search removed all of them (+1.8 improvement)
* **Instance 2:** Greedy created 3 clashes, local search removed all of them (+1.4 improvement)
* **Instance 3:** Greedy created 5 clashes, local search removed 3 of them (+0.9 improvement)

The improvement percentage varies (0.6% to 1.3%), but what really matters is that local search gets rid of conflicts that would be bad for the actual event. Those competing vendor conflicts are probably worth more than the raw score numbers show.

### 3.4 Complexity Summary

| Component | Time Complexity | Space Complexity |
|----|----|----|
| Greedy Construction | O(nm) | O(n + m) |
| Local Search | O(k n²) | O(n + m) |
| **Total** | **O(nm + k n²)** | **O(n + m)** |

When m and n are about the same size, greedy becomes O(n²) and the total is O(n² + k n²) = O(k n²). But since we stop early, k stays small (20-200 in our tests) so it runs pretty fast.

Space-wise we're just storing the booths and grid, so it's linear in the input size.

### 3.5 Challenges and Limitations

**Stuff that gave us trouble:**


1. **Recalculating scores:** At first we were recalculating the entire score every time we tested a move, which was kind of slow. We kept it that way because it's simpler to understand, but we could make it way faster by only recalculating the parts that changed. A simple improvement would be to store each slot's exposure value and just recheck the 2-4 neighbors when we move something.
2. **Getting stuck:** Local search can get stuck in local optimum. Our greedy starting point is usually good enough that this isn't a huge problem, but if we really wanted the best possible solution we could try running it multiple times with different random seeds and pick the best one.
3. **Choosing weights:** We just picked 0.6 and 0.3 based on what seemed reasonable (row clashes should matter more than across-aisle clashes). In a real application we would probably want to make these configurable or tune them based on actual event feedback.

**What doesn't work yet:**

* Can't guarantee we found the absolute best solution (but that's expected for hard combinatorial problems with huge search spaces)
* The weights are hardcoded
* We assume all rows face each other uniformly (might not be true for weird layouts)

**Ideas for improvement:**

* Implement the faster score calculation (only recalculate affected parts)
* Handle more complex floor layouts
* Have some booths that have fixed assginments
* Make a GUI so we can actually see the layouts instead of just text

### 3.6 Practical Impact

If this was actually deployed, it could save event organizers a lot of time. Based on the internship experience, manual planning for a 50-booth event takes a full day of work. Our algorithm does it in under a second and does a better job at avoiding conflicts. For example, in our tests it found and fixed 3-5 vendor clashes that the greedy approach missed.

The algorithm also works well even when the floor is almost completely full (like the 97% full instance). This is important because real events usually want to maximize the number of vendors, so they pack the floor pretty tight.

Also, having the seed printed out is useful if someone wants to reproduce a specific layout (like for a demo or to debug something), they just run it with the same seed and get the exact same result.

## 4. Conclusion

We built a working booth layout optimizer using greedy construction plus local search. The results show it works well:

* Runs in under 0.5 seconds even for 50 booths
* Local search removes conflicts (3-5 clashes eliminated per instance)
* Improves greedy solutions by 0.9-1.8 points

  \

The main thing we learned is that even though this is a hard optimization problem with a massive search space, we can still get pretty good results without having to be super fancy. The greedy algorithm does most of the work by giving high-value booths first choice, and then local search fixes conflicts that happened when greedy focused too much on getting booths into premium spots.

**Who did what:**

* **Alex Eskenazi:** Designed the overall algorithm, implemented greedy, set up the testing, wrote documentation
* **Vishil Patel:** Implemented local search, wrote the scoring functions, handled input/output, worked on optimizations

## 5. References



1. Course slides, CS375 (Fall 2025): **Greedy algorithms** (Intro); **NP/heuristics and approximations** (NP deck); **Optimization topics** including Linear Programming.
2. Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to Algorithms* (3rd ed.). MIT Press. (Greedy algorithms and local search basics)
3. Burkard, R. E., Dell'Amico, M., & Martello, S. (2012). *Assignment Problems* (Revised Edition). SIAM. (General background on assignment and facility layout problems)


