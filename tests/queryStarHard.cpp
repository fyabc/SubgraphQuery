//
// Created by fanyang on 16-7-7.
//

#include "../graph.h"
#include "../utils.h"

#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    string path =
#include "../config.txt"
    ;

    string N = "100000";
    string p = "2.2";
    string maxConstraintStr = "10";

    size_t maxConstraint = (size_t)atoi(maxConstraintStr.c_str());

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");

    ofstream outFileHard(path + "/result/star_hard_pl_" + N + "_" + p + "_" + maxConstraintStr + ".txt");
    ofstream outFileSoft(path + "/result/star_soft_pl_" + N + "_" + p + "_" + maxConstraintStr + ".txt");

    for (size_t k = 1; k < pG->size(); ++k) {
        if (k % 100 == 0)
            cout << k << endl;

        if (k - 1 > pG->maxDegree())
            break;

        outFileHard << k << "   \t";
        outFileSoft << k << "   \t";

        auto pQ = Graph::createStar(k);
        for (size_t constraint = 1; constraint < maxConstraint; ++constraint) {
            auto resultH = pG->getSubgraphNumber_Star_DegreesHard(*pQ, constraint);
            expRep(resultH, outFileHard);
            outFileHard << " " << resultH.get_str(10).size() << "  \t";

            auto resultS = pG->getSubgraphNumber_Star_DegreesSoft(*pQ, constraint);
            expRep(resultS, outFileSoft);
            outFileSoft << " " << resultS.get_str(10).size() << "  \t";
        }

        outFileHard << endl;
        outFileSoft << endl;
    }

    outFileHard.close();

    return 0;
}