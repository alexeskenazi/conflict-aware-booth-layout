#include "io.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

// Skip comment lines and empty lines
static bool ReadLine(ifstream& in, string& line) {
    while (getline(in, line)) {
        // trim leading whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == string::npos) continue; // empty line
        line = line.substr(start);
        if (line[0] == '#') continue; // comment
        return true;
    }
    return false;
}

Instance ReadInstance(const string& filename, const Params& params) {
    Instance inst;
    inst.params = params;

    ifstream in(filename);
    if (!in) {
        cerr << "Error: cannot open " << filename << endl;
        exit(1);
    }

    string line;

    // Read rows and cols
    if (!ReadLine(in, line)) {
        cerr << "Error: missing rows/cols" << endl;
        exit(1);
    }
    istringstream iss1(line);
    iss1 >> inst.params.rows >> inst.params.cols;

    // Read number of booths
    if (!ReadLine(in, line)) {
        cerr << "Error: missing booth count" << endl;
        exit(1);
    }
    int num_booths;
    istringstream iss2(line);
    iss2 >> num_booths;

    // Read booths
    inst.booths.resize(num_booths);
    for (int i = 0; i < num_booths; i++) {
        if (!ReadLine(in, line)) {
            cerr << "Error: missing booth data" << endl;
            exit(1);
        }
        istringstream iss(line);
        iss >> inst.booths[i].id >> inst.booths[i].category >> inst.booths[i].value;
    }

    // Initialize slots
    int total_slots = inst.params.rows * inst.params.cols;
    inst.slots.resize(total_slots);
    for (int r = 0; r < inst.params.rows; r++) {
        for (int c = 0; c < inst.params.cols; c++) {
            int idx = inst.index(r, c);
            inst.slots[idx].r = r;
            inst.slots[idx].c = c;
            inst.slots[idx].bonus = 0.5; // default
        }
    }

    // Read blocked slots
    if (!ReadLine(in, line)) {
        cerr << "Error: missing blocked count" << endl;
        exit(1);
    }
    int num_blocked;
    istringstream iss3(line);
    iss3 >> num_blocked;

    for (int i = 0; i < num_blocked; i++) {
        if (!ReadLine(in, line)) {
            cerr << "Error: missing blocked slot" << endl;
            exit(1);
        }
        int r, c;
        istringstream iss(line);
        iss >> r >> c;
        inst.slots[inst.index(r, c)].blocked = true;
    }

    // Read position bonuses
    for (int r = 0; r < inst.params.rows; r++) {
        if (!ReadLine(in, line)) {
            cerr << "Error: missing bonus row " << r << endl;
            exit(1);
        }
        istringstream iss(line);
        for (int c = 0; c < inst.params.cols; c++) {
            double bonus;
            iss >> bonus;
            inst.slots[inst.index(r, c)].bonus = bonus;
        }
    }

    // Check if we have enough free slots
    int free_slots = 0;
    for (const auto& s : inst.slots) {
        if (!s.blocked) free_slots++;
    }
    if (free_slots < num_booths) {
        cerr << "Error: not enough slots for all booths" << endl;
        exit(1);
    }

    return inst;
}

void WriteLayout(const string& filename, const Layout& layout) {
    ofstream out(filename);
    if (!out) {
        cerr << "Error: cannot write to " << filename << endl;
        return;
    }

    for (int r = 0; r < layout.inst.params.rows; r++) {
        for (int c = 0; c < layout.inst.params.cols; c++) {
            int idx = layout.inst.index(r, c);
            const Slot& s = layout.inst.slots[idx];

            if (s.blocked) {
                out << "X ";
            } else if (s.booth_id == -1) {
                out << ". ";
            } else {
                out << s.booth_id << " ";
            }
        }
        out << "\n";
    }
}

void PrintLayout(const Layout& layout) {
    for (int r = 0; r < layout.inst.params.rows; r++) {
        for (int c = 0; c < layout.inst.params.cols; c++) {
            int idx = layout.inst.index(r, c);
            const Slot& s = layout.inst.slots[idx];

            if (s.blocked) {
                cout << "  X";
            } else if (s.booth_id == -1) {
                cout << "  .";
            } else {
                cout << setw(3) << s.booth_id;
            }
        }
        cout << "\n";
    }
}
