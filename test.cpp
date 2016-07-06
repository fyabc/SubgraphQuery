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

    string N = "1000000";
    string p = "2.2";
    vector<size_t> branches({0});

    if (argc > 1) {
        assert(argc >= 4);

        N = argv[1];
        p = argv[2];

        branches.clear();
        for (auto i = 3; i < argc; ++i)
            branches.push_back((size_t)atoi(argv[i]));
    }

    auto pG = Graph::fromFile("D:/Paper/data/pl_" + N + "_" + p + ".txt");
//    auto pG = Graph::createTree(1, 100);
//    auto pQ = Graph::createTreeH(branches);
//
//    auto confidence = 0.1;
//    auto error = 0.2;
//
////    auto sampleTime = getSampleTime(pQ->size(), confidence, error);
//    auto sampleTime = pQ->size() * 4;
//
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

    ofstream outFile("D:/Paper/result_pl_" + N + "_" + p + ".txt");

    auto n = pG->size();
    auto maxDegree = pG->maxDegree();
    vector<size_t> degrees;
    vector<mpz_class> numbers(n, mpz_class(1));
    for (size_t i = 0; i < n; ++i)
        degrees.push_back(pG->degree(i));

    for (size_t i = 1; i <= n; ++i) {
        if (i % 100 == 0)
            cout << i << endl;
        if (i > maxDegree)
            outFile << i << "\t0\t0" << endl;
        else {
            mpz_class result = accumulate(numbers.begin(), numbers.end(), mpz_class(0));
            outFile << i << "\t" << result.get_str(10).size() << "\t" << result << endl;

            for (size_t j = 0; j < n; ++j) {
                if (degrees[j] > 0) {
                    numbers[j] *= degrees[j];
                    --degrees[j];
                }
                else if (degrees[j] == 0)
                    numbers[j] = 0;
            }
        }
    }

    outFile.close();

    return 0;
}
