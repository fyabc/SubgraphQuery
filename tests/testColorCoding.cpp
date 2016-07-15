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
    string p = "2.5";

    if (argc >= 2)
        N = argv[1];
    if (argc >= 3)
        p = argv[2];

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");
//    auto pG = unique_ptr<Graph>(new Graph(9));
//    pG->addEdges({0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7},
//                 {1, 6, 7, 3, 7, 8, 5, 8, 6, 7, 8, 8});
//    auto pG = Graph::createComplete(50);

    // A Sample Query.
    Graph Q(5);
    Q.addEdge(0, 1); Q.addEdge(0, 3); Q.addEdge(1, 2); Q.addEdge(2, 3); Q.addEdge(2, 4); Q.addEdge(3, 4);

    vector<Graph::DecomposeTree2Node> decompose(2);
    decompose[0].vertices = {0, 1, 2, 3};
    decompose[0].children = {1};
    decompose[0].annotatedVertices = {-1, -1, -1, -1};
    decompose[0].annotatedEdges = {-1, -1, 1, -1};
    decompose[0].bNodeIndexes = {0, 1};

    decompose[1].vertices = {2, 4, 3};
    decompose[1].children = {};
    decompose[1].annotatedVertices = {-1, -1, -1};
    decompose[1].annotatedEdges = {-1, -1, -1};
    decompose[1].bNodeIndexes = {0, 2};

//    Graph Q = *Graph::createCycle(3);
//
//    vector<Graph::DecomposeTree2Node> decompose(1);
//    decompose[0].vertices = {0, 1, 2};
//    decompose[0].children = {};
//    decompose[0].annotatedVertices = {-1, -1, -1};
//    decompose[0].annotatedEdges = {-1, -1, -1};
//    decompose[0].bNodeIndexes = {0, 2};

    clock_t timeBefore, timeAfter;
    mpz_class result;

//    cout << "G: Power-law(" << N << ", " << p << "), N = " << N << ", E = " << pG->edgeNum() << endl;
    pG->showGraphInfo();

    for (auto i = 1; i <= 10; ++i) {
        {
            timeBefore = clock();
            result = pG->getSubgraphNumber_2Treewidth_Decompose(Q, decompose, 1);
            timeAfter = clock();

            cout << "[ColorCoding]  " << result << " " << result.get_str(10).size() << endl;
            cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
        }

//        {
//            timeBefore = clock();
//            result = pG->getSubgraphNumber_Triangle();
//            timeAfter = clock();
//
//            cout << "[Triangle]" << result << " " << result.get_str(10).size() << endl;
//            cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
//        }

//        {
//            timeBefore = clock();
//            result = pG->getSubgraphNumber_BF(Q, 1);
//            timeAfter = clock();
//
//            cout << "[BF]" << result << " " << result.get_str(10).size() << endl;
//            cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
//        }
//
//        {
//            auto testNum = 10000;
//
//            timeBefore = clock();
//            result = pG->testSubgraph(Q, testNum) * permute(pG->size(), Q.size()) / testNum;
//            timeAfter = clock();
//
//            cout << "[Sample]       " << result << " " << result.get_str(10).size() << endl;
//            cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;
//        }
    }

    return 0;
}