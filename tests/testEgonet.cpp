//
// Created by fanyang on 7/21/2016.
//

#include "../graph.h"
#include "../utils.h"

#include <ctime>

using namespace std;

int main(int argc, char** argv) {
    srand((unsigned int)time(nullptr));

    auto path = getDataPath();

    string fileName = "Arxiv-CondMat.txt";
    string egonetFileName = "Q2.txt";
    int testTimes = 10;

    if (argc >= 2) {
        fileName = argv[1];
    }
    if (argc >= 3) {
        egonetFileName = argv[2];
    }
    if (argc >= 4) {
        testTimes = atoi(argv[3]);
    }


    auto pG = Graph::fromFileByNamedVertices(path + "/data/" + fileName);

    pG->showGraphInfo();

    auto pEgonet = Graph::fromFileByNamedVertices(path + "/data/" + egonetFileName, "GeneratedEgonet");

    pEgonet->showGraphInfo();
    pEgonet->showAdj();

    vector<Graph::DecomposeTree2Node> decompose;

    auto haveDecompose = pEgonet->decomposeEgonet(decompose);

    if (!haveDecompose) {
        cout << "Treewidth > 2, do not have decomposition!" << endl;
        return 0;
    }

    for (int i = 1; i <= testTimes; ++i) {
        mpz_class result(0);

        auto timeBefore = clock();
        if (pEgonet->isStar())
            result = pG->getSubgraphNumber_Star(*pEgonet);
        else
            result = pG->getSubgraphNumber_2Treewidth_Decompose(*pEgonet, decompose, 1);
        auto timeAfter = clock();

        cout << "[Color Coding] " << result << " " << result.get_str(10).size() << endl;
        cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
    }

    return 0;
}