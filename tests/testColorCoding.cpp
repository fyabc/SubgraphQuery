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

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");

    auto timeBefore = clock();
    auto result = pG->getSubgraphNumber_Tree(*(Graph::createTreeH({2, 3})), 1);
    auto timeAfter = clock();

    cout << result << " " << result.get_str(10).size() << endl;
    cout << "Time: " << double(timeAfter - timeBefore) / CLOCKS_PER_SEC << "s" << endl;

    return 0;
}