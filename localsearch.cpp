#include "localsearch.h"
#include "score.h"
#include <random>
#include <vector>
#include <algorithm>

using namespace std;

void LocalSearch(Layout& L, int max_iters, unsigned seed) {
    mt19937 rng(seed);

    for (int iter = 0; iter < max_iters; iter++) {
        double best_delta = 0.0;
        int best_move_type = -1; // 0 = swap, 1 = relocate
        int best_booth_a = -1;
        int best_booth_b = -1;
        int best_slot = -1;

        // Collect placed booths
        vector<int> placed_booths;
        for (const auto& s : L.inst.slots) {
            if (s.booth_id >= 0) {
                placed_booths.push_back(s.booth_id);
            }
        }

        // Collect empty slots
        vector<int> empty_slots;
        for (size_t i = 0; i < L.inst.slots.size(); i++) {
            if (!L.inst.slots[i].blocked && L.inst.slots[i].booth_id == -1) {
                empty_slots.push_back(i);
            }
        }

        // Try swap moves (limit to same row or adjacent rows for speed)
        for (size_t i = 0; i < placed_booths.size(); i++) {
            int booth_a = placed_booths[i];
            int pos_a = -1;
            for (size_t k = 0; k < L.inst.slots.size(); k++) {
                if (L.inst.slots[k].booth_id == booth_a) {
                    pos_a = k;
                    break;
                }
            }
            if (pos_a < 0) continue;

            int row_a = L.inst.slots[pos_a].r;

            for (size_t j = i + 1; j < placed_booths.size(); j++) {
                int booth_b = placed_booths[j];
                int pos_b = -1;
                for (size_t k = 0; k < L.inst.slots.size(); k++) {
                    if (L.inst.slots[k].booth_id == booth_b) {
                        pos_b = k;
                        break;
                    }
                }
                if (pos_b < 0) continue;

                int row_b = L.inst.slots[pos_b].r;

                // Only consider swaps within same row or adjacent rows
                if (abs(row_a - row_b) > 1) continue;

                double delta = DeltaMoveSwap(L, booth_a, booth_b);
                if (delta > best_delta) {
                    best_delta = delta;
                    best_move_type = 0;
                    best_booth_a = booth_a;
                    best_booth_b = booth_b;
                }
            }
        }

        // Try relocate moves (sample empty slots for speed)
        int num_empty_samples = min(30, (int)empty_slots.size());
        vector<int> sampled_empties = empty_slots;
        shuffle(sampled_empties.begin(), sampled_empties.end(), rng);
        if (sampled_empties.size() > (size_t)num_empty_samples) {
            sampled_empties.resize(num_empty_samples);
        }

        for (int booth_id : placed_booths) {
            for (int slot_idx : sampled_empties) {
                double delta = DeltaMoveRelocate(L, booth_id, slot_idx);
                if (delta > best_delta) {
                    best_delta = delta;
                    best_move_type = 1;
                    best_booth_a = booth_id;
                    best_slot = slot_idx;
                }
            }
        }

        // Apply best move if any improvement found
        if (best_delta > 0.0) {
            if (best_move_type == 0) {
                // Swap
                int pos_a = -1, pos_b = -1;
                for (size_t i = 0; i < L.inst.slots.size(); i++) {
                    if (L.inst.slots[i].booth_id == best_booth_a) pos_a = i;
                    if (L.inst.slots[i].booth_id == best_booth_b) pos_b = i;
                }
                if (pos_a >= 0 && pos_b >= 0) {
                    L.inst.slots[pos_a].booth_id = best_booth_b;
                    L.inst.slots[pos_b].booth_id = best_booth_a;
                }
            } else if (best_move_type == 1) {
                // Relocate
                int pos_a = -1;
                for (size_t i = 0; i < L.inst.slots.size(); i++) {
                    if (L.inst.slots[i].booth_id == best_booth_a) {
                        pos_a = i;
                        break;
                    }
                }
                if (pos_a >= 0) {
                    L.inst.slots[pos_a].booth_id = -1;
                    L.inst.slots[best_slot].booth_id = best_booth_a;
                }
            }
        } else {
            // No improving move found, stop
            break;
        }
    }
}
