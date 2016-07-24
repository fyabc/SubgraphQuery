//
// Created by fanyang on 16-7-24.
//

#include "../graph.h"
#include "../utils.h"

#include <fstream>

using namespace std;

int main(int argc, char** argv) {
    auto path = getDataPath();

    string Ns[] = {
        "1000", "10000", "100000", "1000000"
    };

    for (const auto& N: Ns) {
        auto pG21 = Graph::fromFile(path + "/data/pl_" + N + "_2.1.txt");
        auto pG25 = Graph::fromFile(path + "/data/pl_" + N + "_2.5.txt");
        auto pG29 = Graph::fromFile(path + "/data/pl_" + N + "_2.9.txt");

        auto max21 = pG21->maxDegree(), max25 = pG25->maxDegree(), max29 = pG29->maxDegree();

        ofstream outFile((path + "/result/star_" + N + "_2.159.txt").c_str());

        cout << "Will up to: " << max21 << endl;

        for (size_t i = 1; i < 1000000; ++i) {
            if (i - 1 > max21 + 2)
                break;

            if (i % 200 == 0)
                cout << i << endl;

            auto result21 = pG21->getSubgraphNumber_Star(i).get_str(10);
            auto result25 = pG25->getSubgraphNumber_Star(i).get_str(10);
            auto result29 = pG29->getSubgraphNumber_Star(i).get_str(10);
            outFile << result21.substr(0, 3) << " " << result21.size() << " ";
            outFile << result25.substr(0, 3) << " " << result25.size() << " ";
            outFile << result29.substr(0, 3) << " " << result29.size() << " ";
            outFile << endl;
        }

        outFile.close();
    }


    return 0;
}