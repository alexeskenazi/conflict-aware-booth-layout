#ifndef GREEDY_H
#define GREEDY_H

#include "model.h"

// Greedy seeding algorithm: place booths one by one in best positions
void GreedySeed(Layout& L);

// Baseline: value-only greedy (ignores clashes)
void GreedyValueOnly(Layout& L);

#endif
