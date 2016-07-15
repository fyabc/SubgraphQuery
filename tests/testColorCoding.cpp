//
// Created by fanyang on 7/11/2016.
//

#include "../graph.h"

#include <fstream>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned int)time(nullptr));

    string path =
#include "../config.txt"
    ;

    string N = "10000";
    string p = "2.2";

    if (argc >= 2)
        N = argv[1];
    if (argc >= 3)
        p = argv[2];

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");

//    // A Sample Query.
//    Graph Q(5);
//    Q.addEdge(0, 1); Q.addEdge(0, 3); Q.addEdge(1, 2); Q.addEdge(2, 3); Q.addEdge(2, 4); Q.addEdge(3, 4);
//
//    vector<Graph::DecomposeTree2Node> decompose(2);
//    decompose[0].vertices = {0, 1, 2, 3};
//    decompose[0].children = {1};
//    decompose[0].annotatedVertices = {-1, -1, -1, -1};
//    decompose[0].annotatedEdges = {-1, -1, 1, -1};
//    decompose[0].bNodeIndexes = {};
//
//    decompose[1].vertices = {2, 4, 3};
//    decompose[1].children = {};
//    decompose[1].annotatedVertices = {-1, -1, -1};
//    decompose[1].annotatedEdges = {-1, -1, -1};
//    decompose[1].bNodeIndexes = {1, 3};

    Graph Q = *Graph::createCycle(3);

    vector<Graph::DecomposeTree2Node> decompose(1);
    decompose[0].vertices = {0, 1, 2};
    decompose[0].children = {};
    decompose[0].annotatedVertices = {-1, -1, -1};
    decompose[0].annotatedEdges = {-1, -1, -1};
    decompose[0].bNodeIndexes = {0, 1};

    clock_t timeBefore, timeAfter;
    mpz_class result;

    for (auto i = 1; i <= 10; ++i) {
        {
            timeBefore = clock();
            result = pG->getSubgraphNumber_2Treewidth_Decompose(Q, decompose, 1);
            timeAfter = clock();

            cout << "[CC]" << result << " " << result.get_str(10).size() << endl;
            cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
        }

        {
            timeBefore = clock();
            result = pG->getSubgraphNumber_Triangle();
            timeAfter = clock();

            cout << "[TA]" << result << " " << result.get_str(10).size() << endl;
            cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
        }

//        {
//            timeBefore = clock();
//            result = pG->getSubgraphNumber_BF(*(Graph::createCycle(3)), 1);
//            timeAfter = clock();
//
//            cout << "[BF]" << result << " " << result.get_str(10).size() << endl;
//            cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
//        }
    }

    return 0;
}