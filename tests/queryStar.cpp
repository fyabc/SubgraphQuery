//
// Created by fanyang on 7/7/2016.
//

#include "../graph.h"
#include "../utils.h"

#include <ctime>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    srand((unsigned)time(nullptr));

    string path =
#include "../config.txt"
    ;

    string N = "1000000";
    string p = "2.2";
    size_t k = 100;
    size_t constraint = 1;
    char buf[22];
    sprintf(buf, "%u", constraint);

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");

    ofstream outFile(path + "/result/star_pl_" + N + "_" + p + "_" + buf + ".txt");

    for (k = 1; k <= pG->size(); ++k) {
        if (k % 100 == 0)
            cout << k << endl;

        if (k - 1 > pG->maxDegree()) {
            outFile << k << "   \t0.E0 0  \t0.E0 0  \t0.E0 0" << endl;
            break;
        }

        auto pQ = Graph::createStar(k);
        auto result = pG->getSubgraphNumber_Star(*pQ);
        auto resultH = pG->getSubgraphNumber_Star_DegreesHard(*pQ, constraint);
        auto resultS = pG->getSubgraphNumber_Star_DegreesSoft(*pQ, constraint);

        outFile << k << "   \t";
        expRep(result, outFile);
        outFile << " " << result.get_str(10).size() << "  \t";
        expRep(resultH, outFile);
        outFile << " " << resultH.get_str(10).size() << "  \t";
        expRep(resultS, outFile);
        outFile << " " << resultS.get_str(10).size() << endl;
    }

    outFile.close();

    return 0;
}