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

    string N = "1000000";
    string p = "2.8";
    size_t b1 = 4, b2 = 40;
    vector<size_t> branches({b1, b2});

    if (argc > 1) {
        assert(argc >= 4);

        N = argv[1];
        p = argv[2];

        branches.clear();
        for (auto i = 3; i < argc; ++i)
            branches.push_back((size_t)atoi(argv[i]));
    }

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");
//    auto pG = Graph::createTree(1, 100);
    auto pQ = Graph::createTreeH(branches);

    auto resultIC = pG->getSubgraphNumber_2dFullTree_ignore(b1, b2);
    cout << "Ignore conflict: " << resultIC << " " << resultIC.get_str(10).size() << endl;

    auto confidence = 0.1;
    auto error = 0.2;
//    auto sampleTime = getSampleTime(pQ->size(), confidence, error);
    auto sampleTime = pQ->size() * 4;

//    mpz_class sum(0);
//    auto timeBefore = clock();
//    for (auto i = 1; i <= sampleTime; ++i) {
//        auto result = pG->getSubgraphNumber_Tree(*pQ, 1);
//        sum += result;
//
//        cout << result << " " << result.get_str(10).size() << " " << sum / i << endl;
//    }
//    auto timeAfter = clock();
//
//    cout << "Time passed: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
//    cout << "Average: " << sum / sampleTime << endl;
//    cout << "Ignore : " << pG->getSubgraphNumber_2dFullTree_ignore(b1, b2) << endl;

    return 0;
}
