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

    string N = "10000";
    string p = "2.2";
    string maxConstraintStr = "100";

    size_t maxConstraint = (size_t)atoi(maxConstraintStr.c_str());

    auto pG = Graph::fromFile(path + "/data/pl_" + N + "_" + p + ".txt");

    ofstream outFileHrHl(path + "/result/star_hr_hl_pl_" + N + "_" + p + "_" + maxConstraintStr + ".txt");
    ofstream outFileHrSl(path + "/result/star_hr_sl_pl_" + N + "_" + p + "_" + maxConstraintStr + ".txt");
    ofstream outFileSrHl(path + "/result/star_sr_hl_pl_" + N + "_" + p + "_" + maxConstraintStr + ".txt");
    ofstream outFileSrSl(path + "/result/star_sr_sl_pl_" + N + "_" + p + "_" + maxConstraintStr + ".txt");

    for (size_t k = 1; k < pG->size(); ++k) {
        if (k % 1 == 0)
            cout << k << endl;

        if (k - 1 > pG->maxDegree() + 1)
            break;

        outFileHrHl << k << "   \t";
        outFileHrSl << k << "   \t";
        outFileSrHl << k << "   \t";
        outFileSrSl << k << "   \t";

        for (size_t constraint = 1; constraint < maxConstraint; ++constraint) {
//            auto resultH = pG->getSubgraphNumber_Star_DegreesHard(*pQ, constraint, k - 1);
//            expRep(resultH, outFileHard);
//            outFileHard << " " << resultH << "  \t";

            auto resultHrHl = pG->getSubgraphNumber_Star_All(k, constraint, true, k - 1, true);
            outFileHrHl << resultHrHl << "  \t";
            auto resultHrSl = pG->getSubgraphNumber_Star_All(k, constraint, true, k - 1, false);
            outFileHrSl << resultHrSl << "  \t";
            auto resultSrHl = pG->getSubgraphNumber_Star_All(k, constraint, false, k - 1, true);
            outFileSrHl << resultSrHl << "  \t";
            auto resultSrSl = pG->getSubgraphNumber_Star_All(k, constraint, false, k - 1, false);
            outFileSrSl << resultSrSl << "  \t";
        }

        outFileHrHl << endl;
        outFileHrSl << endl;
        outFileSrHl << endl;
        outFileSrSl << endl;
    }

    outFileHrHl.close();
    outFileHrSl.close();
    outFileSrHl.close();
    outFileSrSl.close();

    return 0;
}