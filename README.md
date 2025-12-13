# Booth Layout Optimizer

This program optimizes booth placement in a grid layout for trade shows. It tries to put high-value booths in good spots while avoiding putting competing vendors next to each other.

## Project Documents

* **[Project Report](Project_Report.md)** - Full writeup of the algorithm, results, and analysis (CS375 project)
* **[Assignment Prompt](CS375_Term_Project_Prompt.md)** - Original project requirements
* **[Presentation Slides](Project_Slides.md)** - Summary slides for the 6-minute presentation

## How it works

The program uses two steps:


1. Greedy algorithm - places booths one at a time in good positions
2. Local search - swaps booths around to improve the layout

## Building

```bash
make
```

This creates:

* `layout` - the main program
* `test` - runs tests

## Running

```bash
./layout greedy_trap.txt 42
```

Or try other test instances:

```bash
./layout almost_full.txt 42      # 97% full grid
./layout super_tight.txt 42      # 96% full grid with heavy category conflicts
```

The second argument is the random seed (default is random). Using a specific seed makes the results reproducible.

## Input Format

The input file has:

* Grid dimensions (rows and columns)
* List of booths with their category and value
* Blocked slots (info desk, storage, etc.)
* Position bonuses for each grid slot (higher = better location)

See `greedy_trap.txt` or `almost_full.txt` for examples.

## Running Tests

```bash
./test
```

## Scoring

Score = Exposure - (0.6 \* RowClashes) - (0.3 \* AcrossAisleClashes)

Where:

* Exposure = sum of (booth value × position bonus)
* RowClashes = pairs of same-category booths next to each other horizontally
* AcrossAisleClashes = pairs of same-category booths across from each other vertically


