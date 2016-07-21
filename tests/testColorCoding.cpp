//
// Created by fanyang on 7/11/2016.
//

#include "../graph.h"
#include "../utils.h"

#include <sstream>
#include <fstream>
#include <ctime>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned int)time(nullptr));

    auto path = getDataPath();

    string N = "10000";
    string p = "2.5";
    string QName = "cycle3.qd";

    if (argc >= 2)
        N = argv[1];
    if (argc >= 3)
        p = argv[2];
    if (argc >= 4) {
        QName = argv[3];
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

    cerr << "Begin Testing... Current Time is " << (double)clock() / CLOCKS_PER_SEC << "s" << endl;
    logger("Template Name: " + QName + "\n");

    // Check for correction.
    logger("Check complete graph (N = 50) for correction:");
    auto C50 = Graph::createComplete(50);
    mpz_class resultS = permute(C50->size(), Q.size());
    cout << "[Calculate]    " << resultS << " " << resultS.get_str(10).size() << endl;

    for (auto i = 1; i <= 3; ++i) {
        cerr << "Check Time " << i << "(Total " << 3 << ")" << endl;
        auto resultC = C50->getSubgraphNumber_2Treewidth_Decompose(Q, decompose, 1);
        cout << "[Color Coding] " << resultC << " " << resultC.get_str(10).size() << endl;
    }

    logger();

    // Main Test
    logger("Main Test:");
    pG->showGraphInfo();

    logger();

    auto testTimes = 30;

#ifndef _OPENMP
    mpz_class total(0);
    for (auto i = 1; i <= testTimes; ++i) {
		cerr << "Test Time " << i << " (Total " << testTimes << ")" << endl;

        auto timeBefore = clock();
        auto result = pG->getSubgraphNumber_2Treewidth_Decompose(Q, decompose, 1);
        auto timeAfter = clock();

        total += result;
        cout << "[Color Coding] " << result << " " << result.get_str(10).size() << endl;
        cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
        mpz_class avg = total / i;
        cout << "Average: " << avg << " " << avg.get_str(10).size() << endl;
    }
#else

    omp_set_num_threads(omp_get_num_procs() - 1);

#pragma omp parallel for default(shared)
    for (int i = 1; i <= testTimes; ++i) {
        stringstream ss;

        ss << "Test Time " << i << " (Total " << testTimes << ")" << endl;
        cerr << ss.str() << flush;

        auto decomposeIn = decompose;
		auto GIn = *pG;

        auto timeBefore = clock();
        auto result = GIn.getSubgraphNumber_2Treewidth_Decompose(Q, decomposeIn, 1);
        auto timeAfter = clock();

        ss.clear(); ss.str("");
        ss << "[Color Coding] " << result << " " << result.get_str(10).size() << endl;
        ss << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
        cout << ss.str() << flush;
    }
#endif
    cerr << "End Testing... Current Time is " << (double)clock() / CLOCKS_PER_SEC << "s" << endl;

    return 0;
}
