# Booth Layout Optimizer

This program optimizes booth placement in a grid layout for trade shows. It tries to put high-value booths in good spots while avoiding putting competing vendors next to each other.

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
./layout data/small1.txt
```

## Input Format

The input file has:

* Grid dimensions (rows and columns)
* List of booths with their category and value
* Blocked slots (info desk, storage, etc.)
* Position bonuses for each grid slot (higher = better location)

See `data/small1.txt` for an example.

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


