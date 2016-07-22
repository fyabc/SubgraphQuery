//
// Created by fanyang on 7/21/2016.
//

#include "../graph.h"
#include "../utils.h"

#include <ctime>
#include <getopt.h>
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

int getTestTime(size_t k, double epsilon, double delta) {
    double result = 1.0;

    for (size_t i = 1; i <= k; ++i) {
        result *= k;
        result /= i;
    }

    result /= epsilon * epsilon * delta;

    return int(result);
}

struct OptionArgs {
    bool help = false;
    bool showInfo = false;
    string gFileName;
    string qFileName;

    int threadNum = -1;
    int testTimes = -1;

    double epsilon = 0.1;
    double delta = 0.1;
};

void parseOpt(int argc, char* argv[], OptionArgs* optionArgs) {
    const char optString[] = "g:q:e:d:t:p:hi";

    int opt;

    while ((opt = getopt(argc, argv, optString)) != -1) {
        switch (opt) {
            case 'g':
                optionArgs->gFileName = optarg;
                break;
            case 'q':
                optionArgs->qFileName = optarg;
                break;
            case 'e':
                optionArgs->epsilon = atof(optarg);
                break;
            case 'd':
                optionArgs->delta = atof(optarg);
                break;
            case 'h':
                optionArgs->help = true;
                break;
            case 't':
                optionArgs->testTimes = atoi(optarg);
                break;
            case 'p':
                optionArgs->threadNum = atoi(optarg);
            case 'i':
                optionArgs->showInfo = true;
                break;
            default:
                break;
        }
    }
}

void usage() {
    printf(
        "Usage\n"
        "testEgonet -g gFile -q qFile [-e epsilon] [-d delta] [-t testTimes] [-p threadNum] [-i] [-h]\n"
        "\n"
        "Do an Ego Network query.\n"
        "\n"
        "Options:\n"
        "    -g gFile        the file name of graph G\n"
        "    -q qFile        the file name of graph Q\n"
        "    -e epsilon      the value of epsilon (default is 0.1)\n"
        "    -d delta        the value of delta (default is 0.1)\n"
        "    -t testTimes    the number of test times (override -e and -d)\n"
        "    -p threadNum    the number of thread in parallel sampling (warning: more threads will cost more space)\n"
        "    -i              show query details\n"
        "    -h              print this help and exit\n"
        "\n"
    );

    exit(0);
}

int main(int argc, char** argv) {
    srand((unsigned int)time(nullptr));

    OptionArgs optionArgs;
    parseOpt(argc, argv, &optionArgs);

    if (optionArgs.help) {
        usage();
    }

    if (optionArgs.gFileName == "") {
        cerr << "Error: G file name must be given" << endl;
        return 0;
    }
    if (optionArgs.qFileName == "") {
        cerr << "Error: Q file name must be given" << endl;
        return 0;
    }
    auto pG = Graph::fromFileByNamedVertices(optionArgs.gFileName, "RealDataSet");
    auto pEgonet = Graph::fromFileByNamedVertices(optionArgs.qFileName, "GeneratedEgonet");

    if (optionArgs.showInfo) {
        pG->showGraphInfo();
        pEgonet->showGraphInfo();
    }

    if (optionArgs.testTimes == -1) {
        optionArgs.testTimes = getTestTime(pEgonet->size(), optionArgs.epsilon, optionArgs.delta);
    }

    if(optionArgs.showInfo) {
        cout << "Test times = " << optionArgs.testTimes << endl;
    }

    vector<Graph::DecomposeTree2Node> decompose;
    auto haveDecompose = pEgonet->decomposeEgonet(decompose);

    if (!haveDecompose) {
        cout << "Treewidth > 2, do not have decomposition!" << endl;
        return 0;
    }

#ifndef _OPENMP
    if (optionArgs.threadNum > 1) {
        cerr << "The environment does not support OpenMP, thread number is omitted" << endl;
    }

    mpz_class result(0);

    auto timeBefore = clock();
    if (pEgonet->isStar())
        result = pG->getSubgraphNumber_Star(*pEgonet);
    else if (pEgonet->edgeNum() == pEgonet->size())
        result = pG->getSubgraphNumber_StarAnd1Edge(*pEgonet);
    else
        result = pG->getSubgraphNumber_2Treewidth_Decompose(*pEgonet, decompose, optionArgs.testTimes);
    auto timeAfter = clock();

    cout << result << endl;

    if (optionArgs.showInfo) {
        cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
    }

#else
    if (optionArgs.threadNum == -1)
        optionArgs.threadNum = max(1, omp_get_num_procs() - 2);
	if (optionArgs.showInfo) {
		cout << "Thread number = " << optionArgs.threadNum << endl;
	}

    omp_set_num_threads(optionArgs.threadNum);
    mpz_class sum(0);

    auto timeBefore = clock();

    if (pEgonet->isStar())
        sum = pEgonet->getSubgraphNumber_Star(*pEgonet);
    else if (pEgonet->edgeNum() == pEgonet->size())
        sum = pEgonet->getSubgraphNumber_StarAnd1Edge(*pEgonet);
    else {
        mpz_class* total = new mpz_class[optionArgs.testTimes];

#pragma omp parallel for default(shared)
        for (int i = 0; i < optionArgs.testTimes; ++i) {
            auto localEgonet = *pEgonet;
            auto localDecompose = decompose;
            auto localG = *pG;

            total[i] += localG.getSubgraphNumber_2Treewidth_Decompose(localEgonet, localDecompose, 1);
        }

        for (auto i = 0; i < optionArgs.testTimes; ++i)
            sum += total[i];

        sum /= optionArgs.testTimes;

        delete[] total;
    }

    auto timeAfter = clock();

    cout << sum << endl;
    
	if (optionArgs.showInfo) {
        cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
    }
#endif

    return 0;
}
