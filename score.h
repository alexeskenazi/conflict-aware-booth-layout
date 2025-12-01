#ifndef SCORE_H
#define SCORE_H

#include "model.h"

// Compute total exposure gain
double ComputeExposure(const Layout& L);

// Compute row clash count (same category adjacent horizontally)
int ComputeRowClash(const Layout& L);

// Compute across-aisle clash count (same category across aisle)
int ComputeAcrossAisleClash(const Layout& L);

// Compute total score
double ComputeTotalScore(const Layout& L);

// Compute delta score for relocating a booth to a new slot
double DeltaMoveRelocate(const Layout& L, int booth_id, int to_index);

// Compute delta score for swapping two booths
double DeltaMoveSwap(const Layout& L, int booth_id_a, int booth_id_b);

#endif
