//
// Created by fanyang on 16-6-25.
//

#include "graph.h"
#include "utils.h"

#include <cstdio>

using namespace std;

int main() {
    srand((unsigned)time(nullptr));

    auto pG = Graph::fromFile("data/pl_1000000_2.5.txt");
//    auto pG = Graph::createTree(1, 100);
    auto pQ = Graph::createTreeH({2, 1});

    auto confidence = 0.1;
    auto error = 0.2;

//    auto sampleTime = getSampleTime(pQ->size(), confidence, error);
    auto sampleTime = 100;

    mpz_class sum(0);
    auto timeBefore = clock();
    for (auto i = 1; i <= sampleTime; ++i) {
        auto result = pG->getSubgraphNumber_Tree(*pQ, 1);
        sum += result;

        cout << result << " " << result.get_str(10).size() << " " << sum / i << endl;
    }
    auto timeAfter = clock();

    cout << "Time passed: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
    cout << "Average: " << sum / sampleTime << endl;

    return 0;
}
