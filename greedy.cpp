#include "greedy.h"
#include "score.h"
#include <algorithm>
#include <map>

using namespace std;

// Helper: count how many booths have each category
static map<string, int> CountCategories(const vector<Booth>& booths) {
    map<string, int> counts;
    for (const auto& b : booths) {
        counts[b.category]++;
    }
    return counts;
}

// Compare booths for greedy ordering: by value desc, then by category rarity
static bool CompareBooths(const Booth& a, const Booth& b,
                          const map<string, int>& cat_counts) {
    if (a.value != b.value) {
        return a.value > b.value; // higher value first
    }
    // tie-break by rarity (lower count = rarer)
    int count_a = cat_counts.at(a.category);
    int count_b = cat_counts.at(b.category);
    return count_a < count_b;
}

void GreedySeed(Layout& L) {
    // sort booths by value, then rarity
    auto cat_counts = CountCategories(L.inst.booths);
    vector<int> order;
    for (size_t i = 0; i < L.inst.booths.size(); i++) {
        order.push_back(i);
    }
    sort(order.begin(), order.end(), [&](int i, int j) {
        return CompareBooths(L.inst.booths[i], L.inst.booths[j], cat_counts);
    });

    // now place them greedily
    for (int booth_id : order) {
        int best_slot = -1;
        double best_delta = -1e9;

        // Try all feasible slots
        for (size_t slot_idx = 0; slot_idx < L.inst.slots.size(); slot_idx++) {
            const Slot& s = L.inst.slots[slot_idx];
            if (s.blocked || s.booth_id >= 0) continue; // not feasible

            // Temporarily place booth
            L.inst.slots[slot_idx].booth_id = booth_id;

            // Compute delta score
            double delta = DeltaMoveRelocate(L, booth_id, slot_idx);

            // For greedy, we actually want the score contribution, not delta
            // Since the booth is not placed yet, we compute its contribution
            double exposure_gain = L.inst.booths[booth_id].value * s.bonus;

            // Count clashes this placement would create
            int row_clash = 0;
            int across_clash = 0;
            int r = s.r;
            int c = s.c;
            string cat = L.inst.booths[booth_id].category;

            // left
            if (c > 0) {
                int left = L.inst.index(r, c - 1);
                int left_bid = L.inst.slots[left].booth_id;
                if (left_bid >= 0 && L.inst.booths[left_bid].category == cat) {
                    row_clash++;
                }
            }
            // right
            if (c < L.inst.params.cols - 1) {
                int right = L.inst.index(r, c + 1);
                int right_bid = L.inst.slots[right].booth_id;
                if (right_bid >= 0 && L.inst.booths[right_bid].category == cat) {
                    row_clash++;
                }
            }
            // above
            if (r > 0) {
                int above = L.inst.index(r - 1, c);
                int above_bid = L.inst.slots[above].booth_id;
                if (above_bid >= 0 && L.inst.booths[above_bid].category == cat) {
                    across_clash++;
                }
            }
            // below
            if (r < L.inst.params.rows - 1) {
                int below = L.inst.index(r + 1, c);
                int below_bid = L.inst.slots[below].booth_id;
                if (below_bid >= 0 && L.inst.booths[below_bid].category == cat) {
                    across_clash++;
                }
            }

            double contribution = exposure_gain
                                - L.inst.params.wC * row_clash
                                - L.inst.params.wX * across_clash;

            // Remove temporary placement
            L.inst.slots[slot_idx].booth_id = -1;

            // Check if this is the best so far
            bool is_better = false;
            if (contribution > best_delta) {
                is_better = true;
            } else if (contribution == best_delta && best_slot >= 0) {
                // tie-break by bonus, then by (r, c)
                if (s.bonus > L.inst.slots[best_slot].bonus) {
                    is_better = true;
                } else if (s.bonus == L.inst.slots[best_slot].bonus) {
                    if (s.r < L.inst.slots[best_slot].r ||
                        (s.r == L.inst.slots[best_slot].r && s.c < L.inst.slots[best_slot].c)) {
                        is_better = true;
                    }
                }
            }

            if (is_better) {
                best_delta = contribution;
                best_slot = slot_idx;
            }
        }

        // Place the booth in the best slot found
        if (best_slot >= 0) {
            L.inst.slots[best_slot].booth_id = booth_id;
        }
    }
}

void GreedyValueOnly(Layout& L) {
    // Sort booths by value only
    vector<int> order;
    for (size_t i = 0; i < L.inst.booths.size(); i++) {
        order.push_back(i);
    }
    sort(order.begin(), order.end(), [&](int i, int j) {
        return L.inst.booths[i].value > L.inst.booths[j].value;
    });

    // Place booths greedily by exposure only
    for (int booth_id : order) {
        int best_slot = -1;
        double best_exposure = -1;

        for (size_t slot_idx = 0; slot_idx < L.inst.slots.size(); slot_idx++) {
            const Slot& s = L.inst.slots[slot_idx];
            if (s.blocked || s.booth_id >= 0) continue;

            double exposure = L.inst.booths[booth_id].value * s.bonus;
            if (exposure > best_exposure) {
                best_exposure = exposure;
                best_slot = slot_idx;
            }
        }

        if (best_slot >= 0) {
            L.inst.slots[best_slot].booth_id = booth_id;
        }
    }
}
