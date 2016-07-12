//
// Created by fanyang on 7/8/2016.
//

#include "../graph.h"

#include <cstdio>

using namespace std;

int main(int argc, char* argv[]) {
    string path =
#include "../config.txt"
    ;

    // generate 2d trees.
    size_t b1Max = 10;
    size_t b2Max = 30;
    char buf1[15], buf2[15];

    for (size_t b1 = 1; b1 <= b1Max; ++b1)
        for (size_t b2 = 1; b2 <= b2Max; ++b2) {
            sprintf(buf1, "%lu", b1);
            sprintf(buf2, "%lu", b2);
            Graph::createTreeH({b1, b2})->toFile(path + "/data/tree_" + buf1 + "_" + buf2 + ".graph");
        }

    // generate ER graphs.
    size_t nMax = 100;
    for (size_t n = 2; n <= nMax; ++n) {
        sprintf(buf1, "%lu", n);
        auto m = min(2 * n - 1, n * (n - 1) / 2);
        sprintf(buf2, "%lu", m);
        Graph::createER(n, m)->toFile(path + "/data/er_" + buf1 + "_" + buf2 + ".graph");
    }

    return 0;
}