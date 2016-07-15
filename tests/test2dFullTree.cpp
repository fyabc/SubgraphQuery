//
// Created by fanyang on 16-7-7.
//

#include "../graph.h"
#include "../utils.h"

#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned)time(nullptr));

    string path =
#include "../config.txt"
    ;

    string N = "10000";
    string p = "2.2";
    string b1MaxStr = "3";
    string b2MaxStr = "500";

    if (argc >= 2)
        N = argv[1];
    if (argc >= 3)
        p = argv[2];
    if (argc >= 4)
        b1MaxStr = argv[3];
    if (argc >= 5)
        b2MaxStr = argv[4];


    size_t b1Max = (size_t)atoi(b1MaxStr.c_str());
    size_t b2Max = (size_t)atoi(b2MaxStr.c_str());
    size_t threshold = 11;

    vector<size_t> branches(2);

    int sampleTimes = 10;

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");

    ofstream outFile(path + "/result/2dFullTree_pl_" + N + "_" + p + "_" + b1MaxStr + "_" + b2MaxStr + ".txt");

    for (size_t b1 = 1; b1 <= b1Max; ++b1) {
        for (size_t b2 = 1; b2 <= b2Max; ++b2) {
            cout << b1 << " " << b2 << endl;
            branches[0] = b1; branches[1] = b2;

            auto pQ = Graph::createTreeH(branches);

            outFile << b1 << "  \t" << b2 << "  \t";

            auto resultI = pG->getSubgraphNumber_2dFullTree_ignore(b1, b2);
            outFile << resultI << "  \t";

            if (1 + b1 + b1 * b2 <= threshold) {
                auto resultC = pG->getSubgraphNumber_Tree(*pQ, sampleTimes);
                outFile << resultC << "  \t";
            }

            outFile << endl;
        }
    }

    outFile.close();

    return 0;
}