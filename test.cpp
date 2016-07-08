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

    auto pG = Graph::fromFile(path + "/data/pl_100_2.2.txt");
    auto dist = pG->getDegreeDistribution();

    auto k = 6;

    mpz_class result(0);
    for (int i = 0; i < dist.size(); ++i)
        if (dist[i].size() != 0) {
            cout << "(" << permute(i, k - 1) << ") ";
            cout << i << " " << dist[i].size() << endl;
            result += permute(i, k - 1) * dist[i].size();
        }

    cout << "[" << result << "]" << endl;

    return 0;
}
