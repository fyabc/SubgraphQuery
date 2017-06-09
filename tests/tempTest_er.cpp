//
// Created by fanyang on 16-7-24.
//

#include "../graph.h"
#include "../utils.h"

#include <fstream>

using namespace std;

int main(int argc, char** argv) {
    auto path = getDataPath();

    {
        string N = "1000000";

        auto pG1 = Graph::fromFile(path + "/data/er_" + N + "_0.0001.txt");
        auto pG2 = Graph::fromFile(path + "/data/er_" + N + "_0.0002.txt");
        auto pG3 = Graph::fromFile(path + "/data/er_" + N + "_0.0003.txt");


        auto maxD1 = pG1->maxDegree(), maxD2 = pG2->maxDegree(), maxD3 = pG3->maxDegree();

        ofstream outFile((path + "/result/er_" + N + ".txt").c_str());

        cout << "Will up to " << maxD3 << endl;

        for (size_t i = 1; i <= 1000000; ++i) {
            if (i - 1 > maxD3 + 2)
                break;

            if (i % 200 == 0)
                cout << i << endl;

            auto result1 = pG1->getSubgraphNumber_Star(i).get_str(10);
            auto result2 = pG2->getSubgraphNumber_Star(i).get_str(10);
            auto result3 = pG3->getSubgraphNumber_Star(i).get_str(10);

            outFile << result1.substr(0, 3) << " " << result1.size() << " ";
            outFile << result2.substr(0, 3) << " " << result1.size() << " ";
            outFile << result3.substr(0, 3) << " " << result1.size() << " ";

            outFile << endl;
        }

        outFile.close();
    }

    return 0;
}
