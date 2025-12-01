#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

using namespace std;

// Basic booth information
struct Booth {
    int id;
    string category;
    double value;
};

// Grid slot that can hold a booth
struct Slot {
    int r, c;
    double bonus;
    bool blocked = false;
    int booth_id = -1; // -1 means empty
};

// Problem parameters
struct Params {
    int rows, cols;
    double wC = 0.6;  // row clash weight
    double wX = 0.3;  // across-aisle clash weight
    unsigned seed = 0;
};

// Complete problem instance
struct Instance {
    Params params;
    vector<Booth> booths;
    vector<Slot> slots; // size = rows * cols

    // helper to convert (r,c) to flat index
    int index(int r, int c) const {
        return r * params.cols + c;
    }
};

// Layout represents a solution
struct Layout {
    Instance inst;
    // booth positions are stored in inst.slots[...].booth_id
};

#endif
