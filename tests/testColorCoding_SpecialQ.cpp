//
// Created by fanyang on 7/20/2016.
//

#include "../graph.h"
#include "../utils.h"

#include <iostream>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned int)time(nullptr));

    auto path = getDataPath();

    string N = "10000";
    string p = "2.5";
    string wStr = "4";
    string type = "cycle";

    if (argc >= 2)
        N = argv[1];
    if (argc >= 3)
        p = argv[2];
    if (argc >= 4)
        wStr = argv[3];
    if (argc >= 5)
        type = argv[4];

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");

    size_t w = (size_t)atoi(wStr.c_str());
    auto QD = type == "cycle" ? Graph::genQD_Cycle(w) : Graph::genQD_Triangles(w);
    auto& Q = *(QD.first);
    auto& decompose = QD.second;

    cerr << "Begin Testing... Current Time is " << (double)clock() / CLOCKS_PER_SEC << "s" << endl;
    logger("Template type: " + type + ", size: " + wStr + "\n");

    logger("Information of Q:");
    Q.showGraphInfo();

    logger("Information of G:");
    pG->showGraphInfo();

    int testTimes = 3;
    mpz_class total(0);
    double totalTime = 0.0;
    for (int i = 1; i <= testTimes; ++i) {
        cerr << "Test Time " << i << " (Total " << testTimes << ")" << endl;

        auto timeBefore = clock();
        auto result = pG->getSubgraphNumber_2Treewidth_Decompose(Q, decompose, 1);
        auto timeAfter = clock();

        auto timePassed = double(timeAfter - timeBefore) / CLOCKS_PER_SEC;
        cout << "[Color Coding] " << result << " " << result.get_str(10).size() << endl;
        cout << "Time: " << timePassed << "s" << endl;

        total += result;
        totalTime += timePassed;
        mpz_class avg = total / i;

        cout << "Average: " << avg << " " << avg.get_str(10).size() << endl;
        cout << "Average Time: " << totalTime / i << endl;
    }

    cerr << "End Testing... Current Time is " << (double)clock() / CLOCKS_PER_SEC << "s" << endl;

    return 0;
}