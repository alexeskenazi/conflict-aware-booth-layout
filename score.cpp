#include "score.h"

using namespace std;

double ComputeExposure(const Layout& L) {
    double total = 0.0;
    for (const auto& slot : L.inst.slots) {
        if (slot.booth_id >= 0) {
            total += L.inst.booths[slot.booth_id].value * slot.bonus;
        }
    }
    return total;
}

int ComputeRowClash(const Layout& L) {
    int clash = 0;
    for (int r = 0; r < L.inst.params.rows; r++) {
        for (int c = 0; c < L.inst.params.cols - 1; c++) {
            int idx1 = L.inst.index(r, c);
            int idx2 = L.inst.index(r, c + 1);
            int b1 = L.inst.slots[idx1].booth_id;
            int b2 = L.inst.slots[idx2].booth_id;
            if (b1 >= 0 && b2 >= 0) {
                if (L.inst.booths[b1].category == L.inst.booths[b2].category) {
                    clash++;
                }
            }
        }
    }
    return clash;
}

int ComputeAcrossAisleClash(const Layout& L) {
    int clash = 0;
    for (int r = 0; r < L.inst.params.rows - 1; r++) {
        for (int c = 0; c < L.inst.params.cols; c++) {
            int idx1 = L.inst.index(r, c);
            int idx2 = L.inst.index(r + 1, c);
            int b1 = L.inst.slots[idx1].booth_id;
            int b2 = L.inst.slots[idx2].booth_id;
            if (b1 >= 0 && b2 >= 0) {
                if (L.inst.booths[b1].category == L.inst.booths[b2].category) {
                    clash++;
                }
            }
        }
    }
    return clash;
}

double ComputeTotalScore(const Layout& L) {
    double exposure = ComputeExposure(L);
    int row_clash = ComputeRowClash(L);
    int across_clash = ComputeAcrossAisleClash(L);
    return exposure - L.inst.params.wC * row_clash - L.inst.params.wX * across_clash;
}

// get category at a slot
static string GetCategory(const Layout& L, int slot_idx) {
    int bid = L.inst.slots[slot_idx].booth_id;
    if (bid < 0) return "";
    return L.inst.booths[bid].category;
}

// count clashes at a slot
static int CountClashesAtSlot(const Layout& L, int slot_idx) {
    int r = L.inst.slots[slot_idx].r;
    int c = L.inst.slots[slot_idx].c;
    string cat = GetCategory(L, slot_idx);
    if (cat.empty()) return 0;

    int clash = 0;

    // Check left neighbor
    if (c > 0) {
        int left_idx = L.inst.index(r, c - 1);
        if (GetCategory(L, left_idx) == cat) clash++;
    }
    // Check right neighbor
    if (c < L.inst.params.cols - 1) {
        int right_idx = L.inst.index(r, c + 1);
        if (GetCategory(L, right_idx) == cat) clash++;
    }
    // Check above
    if (r > 0) {
        int above_idx = L.inst.index(r - 1, c);
        if (GetCategory(L, above_idx) == cat) clash++;
    }
    // Check below
    if (r < L.inst.params.rows - 1) {
        int below_idx = L.inst.index(r + 1, c);
        if (GetCategory(L, below_idx) == cat) clash++;
    }

    return clash;
}

double DeltaMoveRelocate(const Layout& L, int booth_id, int to_index) {
    // Find current position of booth
    int from_index = -1;
    for (size_t i = 0; i < L.inst.slots.size(); i++) {
        if (L.inst.slots[i].booth_id == booth_id) {
            from_index = i;
            break;
        }
    }
    if (from_index < 0) return 0.0; // booth not placed

    const Booth& booth = L.inst.booths[booth_id];

    // Compute old contribution
    double old_exposure = booth.value * L.inst.slots[from_index].bonus;
    int old_row_clash = 0;
    int old_across_clash = 0;

    // Count clashes at old position
    int r_old = L.inst.slots[from_index].r;
    int c_old = L.inst.slots[from_index].c;

    // left
    if (c_old > 0) {
        int left = L.inst.index(r_old, c_old - 1);
        if (GetCategory(L, left) == booth.category) old_row_clash++;
    }
    // right
    if (c_old < L.inst.params.cols - 1) {
        int right = L.inst.index(r_old, c_old + 1);
        if (GetCategory(L, right) == booth.category) old_row_clash++;
    }
    // above
    if (r_old > 0) {
        int above = L.inst.index(r_old - 1, c_old);
        if (GetCategory(L, above) == booth.category) old_across_clash++;
    }
    // below
    if (r_old < L.inst.params.rows - 1) {
        int below = L.inst.index(r_old + 1, c_old);
        if (GetCategory(L, below) == booth.category) old_across_clash++;
    }

    // Compute new contribution
    double new_exposure = booth.value * L.inst.slots[to_index].bonus;
    int new_row_clash = 0;
    int new_across_clash = 0;

    int r_new = L.inst.slots[to_index].r;
    int c_new = L.inst.slots[to_index].c;

    // left
    if (c_new > 0) {
        int left = L.inst.index(r_new, c_new - 1);
        if (GetCategory(L, left) == booth.category) new_row_clash++;
    }
    // right
    if (c_new < L.inst.params.cols - 1) {
        int right = L.inst.index(r_new, c_new + 1);
        if (GetCategory(L, right) == booth.category) new_row_clash++;
    }
    // above
    if (r_new > 0) {
        int above = L.inst.index(r_new - 1, c_new);
        if (GetCategory(L, above) == booth.category) new_across_clash++;
    }
    // below
    if (r_new < L.inst.params.rows - 1) {
        int below = L.inst.index(r_new + 1, c_new);
        if (GetCategory(L, below) == booth.category) new_across_clash++;
    }

    double delta_exposure = new_exposure - old_exposure;
    int delta_row_clash = new_row_clash - old_row_clash;
    int delta_across_clash = new_across_clash - old_across_clash;

    return delta_exposure - L.inst.params.wC * delta_row_clash
                          - L.inst.params.wX * delta_across_clash;
}

double DeltaMoveSwap(const Layout& L, int booth_id_a, int booth_id_b) {
    // Find positions
    int pos_a = -1, pos_b = -1;
    for (size_t i = 0; i < L.inst.slots.size(); i++) {
        if (L.inst.slots[i].booth_id == booth_id_a) pos_a = i;
        if (L.inst.slots[i].booth_id == booth_id_b) pos_b = i;
    }
    if (pos_a < 0 || pos_b < 0) return 0.0;

    const Booth& booth_a = L.inst.booths[booth_id_a];
    const Booth& booth_b = L.inst.booths[booth_id_b];

    // Compute old exposure
    double old_exp = booth_a.value * L.inst.slots[pos_a].bonus
                   + booth_b.value * L.inst.slots[pos_b].bonus;

    // Compute new exposure
    double new_exp = booth_a.value * L.inst.slots[pos_b].bonus
                   + booth_b.value * L.inst.slots[pos_a].bonus;

    // For clashes we need to count before and after
    // Just recompute everything with a temp copy to keep it simple

    // Temp copy for the swap
    Layout temp = L;
    temp.inst.slots[pos_a].booth_id = booth_id_b;
    temp.inst.slots[pos_b].booth_id = booth_id_a;

    double old_score = ComputeTotalScore(L);
    double new_score = ComputeTotalScore(temp);

    return new_score - old_score;
}
