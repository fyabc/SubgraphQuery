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

    auto nStr = "10000", b1MaxStr = "3", b2MaxStr = "500";
    size_t b1Max = (size_t)atoi(b1MaxStr), b2Max = (size_t)atoi(b2MaxStr);

    auto pG = Graph::fromFile(path + "/data/pl_" + nStr + "_2.2.txt");

    ofstream rateFile(path + "/result/rates_pl_" + nStr + "_2.2_" + b1MaxStr + "_" + b2MaxStr + ".txt");
    for (size_t b1 = 1; b1 <= b1Max; ++b1)
        for (size_t b2 = 1; b2 <= b2Max; ++b2) {
            auto result = pG->testSubgraph_2dTree(b1, b2, int(pow(b1, 2.0) * b2 * 150));
            rateFile << b1 << "  \t" << b2 << "  \t" << result << endl;
            cout << b1 << "  \t" << b2 << "  \t" << result << endl;
            if (result * int(pow(b1, 2.0) * b2 * 150) < 1)
                break;
        }
    rateFile.close();

//    auto pQ = Graph::createStar(100);
//    for (size_t i = 1; i <= 1; ++i) {
//        auto result = pG->getSubgraphNumber_Star_DegreesHard(*pQ, i);
//        cout << endl;
//    }

    return 0;
}
