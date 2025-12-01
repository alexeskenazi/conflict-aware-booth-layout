#include "model.h"
#include "io.h"
#include "score.h"
#include "greedy.h"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace std;

// Test 1: Parse instance correctly
void test_parse_instance() {
    Params params;
    Instance inst = ReadInstance("data/small1.txt", params);
    assert(inst.params.rows == 4);
    assert(inst.params.cols == 8);
    assert(inst.booths.size() == 10);
    cout << "Test 1 passed: Instance parsing" << endl;
}

// Test 2: Empty layout has score 0
void test_empty_layout_score() {
    Params params;
    params.rows = 3;
    params.cols = 3;

    Instance inst;
    inst.params = params;
    inst.slots.resize(9);
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int idx = r * 3 + c;
            inst.slots[idx].r = r;
            inst.slots[idx].c = c;
            inst.slots[idx].bonus = 0.5;
            inst.slots[idx].booth_id = -1;
        }
    }

    Layout layout;
    layout.inst = inst;

    double score = ComputeTotalScore(layout);
    assert(score == 0.0);
    cout << "Test 2 passed: Empty layout score is 0" << endl;
}

// Test 3: Row clash detection
void test_row_clash() {
    Params params;
    params.rows = 1;
    params.cols = 3;

    Instance inst;
    inst.params = params;

    // Two booths with same category
    inst.booths.resize(2);
    inst.booths[0].id = 0;
    inst.booths[0].category = "Food";
    inst.booths[0].value = 10;
    inst.booths[1].id = 1;
    inst.booths[1].category = "Food";
    inst.booths[1].value = 10;

    inst.slots.resize(3);
    for (int c = 0; c < 3; c++) {
        inst.slots[c].r = 0;
        inst.slots[c].c = c;
        inst.slots[c].bonus = 0.5;
        inst.slots[c].booth_id = -1;
    }

    // Place both booths next to each other
    inst.slots[0].booth_id = 0;
    inst.slots[1].booth_id = 1;

    Layout layout;
    layout.inst = inst;

    int clash = ComputeRowClash(layout);
    assert(clash == 1);
    cout << "Test 3 passed: Row clash detection" << endl;
}

// Test 4: Across-aisle clash detection
void test_across_aisle_clash() {
    Params params;
    params.rows = 2;
    params.cols = 1;

    Instance inst;
    inst.params = params;

    // Two booths with same category
    inst.booths.resize(2);
    inst.booths[0].id = 0;
    inst.booths[0].category = "Tech";
    inst.booths[0].value = 10;
    inst.booths[1].id = 1;
    inst.booths[1].category = "Tech";
    inst.booths[1].value = 10;

    inst.slots.resize(2);
    inst.slots[0].r = 0;
    inst.slots[0].c = 0;
    inst.slots[0].bonus = 0.5;
    inst.slots[0].booth_id = 0;

    inst.slots[1].r = 1;
    inst.slots[1].c = 0;
    inst.slots[1].bonus = 0.5;
    inst.slots[1].booth_id = 1;

    Layout layout;
    layout.inst = inst;

    int clash = ComputeAcrossAisleClash(layout);
    assert(clash == 1);
    cout << "Test 4 passed: Across-aisle clash detection" << endl;
}

// Test 5: Greedy places high-value booth in best slot
void test_greedy_best_slot() {
    Params params;
    params.rows = 1;
    params.cols = 3;

    Instance inst;
    inst.params = params;

    // One booth
    inst.booths.resize(1);
    inst.booths[0].id = 0;
    inst.booths[0].category = "Test";
    inst.booths[0].value = 10;

    inst.slots.resize(3);
    inst.slots[0].r = 0;
    inst.slots[0].c = 0;
    inst.slots[0].bonus = 0.3;
    inst.slots[0].booth_id = -1;

    inst.slots[1].r = 0;
    inst.slots[1].c = 1;
    inst.slots[1].bonus = 0.9; // best slot
    inst.slots[1].booth_id = -1;

    inst.slots[2].r = 0;
    inst.slots[2].c = 2;
    inst.slots[2].bonus = 0.5;
    inst.slots[2].booth_id = -1;

    Layout layout;
    layout.inst = inst;

    GreedySeed(layout);

    // Booth should be in slot 1 (highest bonus)
    assert(layout.inst.slots[1].booth_id == 0);
    cout << "Test 5 passed: Greedy places booth in best slot" << endl;
}

int main() {
    cout << "Running sanity tests..." << endl;

    test_parse_instance();
    test_empty_layout_score();
    test_row_clash();
    test_across_aisle_clash();
    test_greedy_best_slot();

    cout << "\nAll tests passed!" << endl;
    return 0;
}
