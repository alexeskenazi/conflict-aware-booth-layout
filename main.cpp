#include "model.h"
#include "io.h"
#include "score.h"
#include "greedy.h"
#include "localsearch.h"
#include <iostream>
#include <string>
#include <random>
#include <ctime>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: ./layout <input_file> [seed]" << endl;
        return 1;
    }

    string filename = argv[1];

    Params params;
    params.wC = 0.6;
    params.wX = 0.3;

    // seed for reproducibility
    if (argc >= 3) {
        params.seed = (unsigned)atoi(argv[2]);
    } else {
        params.seed = (unsigned)time(nullptr);
    }
    cout << "Using seed: " << params.seed << endl;

    Instance inst = ReadInstance(filename, params);
    Layout layout;
    layout.inst = inst;

    // run greedy then local search
    cout << "Running greedy..." << endl;
    GreedySeed(layout);

    cout << "Running local search..." << endl;
    LocalSearch(layout, 10000, params.seed);

    // print results
    double exposure = ComputeExposure(layout);
    int row_clash = ComputeRowClash(layout);
    int across_clash = ComputeAcrossAisleClash(layout);
    double total_score = ComputeTotalScore(layout);

    cout << "\nFinal Score: " << total_score << endl;
    cout << "Exposure: " << exposure << endl;
    cout << "Row Clashes: " << row_clash << endl;
    cout << "Across Clashes: " << across_clash << endl;
    cout << endl;

    PrintLayout(layout);

    return 0;
}
