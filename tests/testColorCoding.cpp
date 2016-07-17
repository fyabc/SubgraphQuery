//
// Created by fanyang on 7/11/2016.
//

#include "../graph.h"
#include "../utils.h"

#include <fstream>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned int)time(nullptr));

    string path =
#include "../config.txt"
    ;

    string N = "10000";
    string p = "2.6";
    string QName = "brain3.qd";
//    vector<string> QNames = {"brain3.qd"};

    if (argc >= 2)
        N = argv[1];
    if (argc >= 3)
        p = argv[2];
    if (argc >= 4) {
        QName = argv[3];

//        QNames.clear();
//        for (auto i = 3; i < argc; ++i)
//            QNames.push_back(argv[i]);
    }

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");
//    auto pG = unique_ptr<Graph>(new Graph(9));
//    pG->addEdges({0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7},
//                 {1, 6, 7, 3, 7, 8, 5, 8, 6, 7, 8, 8});
//    auto pG = Graph::createComplete(50);

    // A Sample Query.
    auto QD = Graph::readQueryWithDecompose(QName);
    const auto& Q = *(QD.first);
    auto decompose = QD.second;

    // Check for correction.
    logger("Check complete graph (N = 50) for correction:");
    auto C50 = Graph::createComplete(50);
    auto testNum = 10000;
    mpz_class resultS = C50->testSubgraph(Q, testNum) * permute(C50->size(), Q.size()) / testNum;
    cout << "[Sample]       " << resultS << " " << resultS.get_str(10).size() << endl;

    for (auto i = 1; i <= 3; ++i) {
        auto resultC = C50->getSubgraphNumber_2Treewidth_Decompose(Q, decompose, 1);
        cout << "[Color Coding] " << resultC << " " << resultC.get_str(10).size() << endl;
    }

    logger();

    // Main Test
    logger("Main Test:");
    pG->showGraphInfo();

    logger();

    auto testTimes = 25;

    mpz_class total(0);
    for (auto i = 1; i < testTimes; ++i) {
        auto timeBefore = clock();
        auto result = pG->getSubgraphNumber_2Treewidth_Decompose(Q, decompose, 1);
        auto timeAfter = clock();

        total += result;
        cout << "[Color Coding] " << result << " " << result.get_str(10).size() << endl;
        cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
        mpz_class avg = total / i;
        cout << "Average: " << avg << " " << avg.get_str(10).size() << endl;
    }

    return 0;
}
