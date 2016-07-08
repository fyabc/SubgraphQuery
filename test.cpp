//
// Created by fanyang on 16-6-25.
//

#include "graph.h"
#include "utils.h"

#include <ctime>
#include <cassert>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned)time(nullptr));

    string path =
#include "config.txt"
    ;

    auto pG = Graph::fromFile(path + "/data/pl_10000_2.2.txt");
    size_t b1Max = 3, b2Max = 100;
    int sampleTimes = 1500;

    auto result = pG->testSubgraph_2dTree(b1Max, b2Max, sampleTimes);
    cout << "Total: " << result << " of " << sampleTimes << endl;

    ofstream rateFile(path + "/result/rates_pl_10000_2.2_3_50.txt");
    for (size_t b1 = 1; b1 <= b1Max; ++b1)
        for (size_t b2 = 1; b2 <= b2Max; ++b2) {
            auto result = pG->testSubgraph_2dTree(b1, b2, b1 * b2 * 300);
            rateFile << b1 << "  \t" << b2 << "  \t" << result << endl;
            cout << b1 << "  \t" << b2 << "  \t" << result << endl;
        }
    rateFile.close();

    return 0;
}
