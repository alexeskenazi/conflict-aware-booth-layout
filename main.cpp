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

    // run greedy
    cout << "Running greedy..." << endl;
    GreedySeed(layout);

    double greedy_score = ComputeTotalScore(layout);
    int greedy_row_clash = ComputeRowClash(layout);
    int greedy_across_clash = ComputeAcrossAisleClash(layout);

    cout << "Greedy Score: " << greedy_score << endl;
    cout << "  Row Clashes: " << greedy_row_clash << endl;
    cout << "  Across Clashes: " << greedy_across_clash << endl;

    // run local search
    cout << "\nRunning local search..." << endl;
    LocalSearch(layout, 10000, params.seed);

    // print results
    double exposure = ComputeExposure(layout);
    int row_clash = ComputeRowClash(layout);
    int across_clash = ComputeAcrossAisleClash(layout);
    double total_score = ComputeTotalScore(layout);

    cout << "\nFinal Score: " << total_score << endl;
    cout << "  Row Clashes: " << row_clash << endl;
    cout << "  Across Clashes: " << across_clash << endl;
    cout << "\nImprovement: +" << (total_score - greedy_score) << endl;
    cout << "  Clashes removed: " << ((greedy_row_clash + greedy_across_clash) - (row_clash + across_clash)) << endl;
    cout << "Exposure: " << exposure << endl;
    cout << endl;

    PrintLayout(layout);

    return 0;
}
