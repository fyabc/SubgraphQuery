//
// Created by fanyang on 16-7-24.
//

#include "../graph.h"
#include "../utils.h"

#include <fstream>

using namespace std;

int main(int argc, char** argv) {
    auto path = getDataPath();

/*    {   
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
  */  
    {
        auto pGA = Graph::fromFileByNamedVertices(path + "/data/Arxiv-CondMat.txt");
        auto pGY = Graph::fromFileByNamedVertices(path + "/data/YouTube.txt");
        auto pGL = Graph::fromFileByNamedVertices(path + "/data/LiveJournal.txt");
        auto pGD = Graph::fromFileByNamedVertices(path + "/data/DBLP.txt");
        auto pGF = Graph::fromFileByNamedVertices(path + "/data/Facebook.txt");

        auto maxD = pGA->maxDegree();
        if (pGY->maxDegree() > maxD)
            maxD = pGY->maxDegree();
        if (pGL->maxDegree() > maxD)
            maxD = pGL->maxDegree();
        if (pGD->maxDegree() > maxD)
            maxD = pGD->maxDegree();
        if (pGF->maxDegree() > maxD)
            maxD = pGF->maxDegree();

        auto minN = pGA->size();
        if (pGY->size() < minN)
            minN = pGY->size();
        if (pGL->size() < minN)
            minN = pGL->size();
        if (pGD->size() < minN)
            minN = pGD->size();
        if (pGF->size() < minN)
            minN = pGF->size();

        ofstream outFile((path + "/result/real.txt").c_str());

        cout << "Will up to " << maxD << endl;
		cout << "MinN = " << minN << endl;

        for (size_t i = 1; i < maxD + 10; ++i) {
            if (i - 1 > maxD + 2)
                break;

            if (i % 200 == 0)
                cout << i << endl;

            auto resultA = pGA->getSubgraphNumber_Star(i).get_str(10);
            auto resultY = pGY->getSubgraphNumber_Star(i).get_str(10);
            auto resultL = pGL->getSubgraphNumber_Star(i).get_str(10);
            auto resultD = pGD->getSubgraphNumber_Star(i).get_str(10);
            auto resultF = pGF->getSubgraphNumber_Star(i).get_str(10);

            outFile << resultA.substr(0, 3) << " " << resultA.size() << " ";
            outFile << resultY.substr(0, 3) << " " << resultY.size() << " ";
            outFile << resultL.substr(0, 3) << " " << resultL.size() << " ";
            outFile << resultD.substr(0, 3) << " " << resultD.size() << " ";
            outFile << resultF.substr(0, 3) << " " << resultF.size() << " ";

            outFile << endl;
        }

        outFile.close();
    }

    return 0;
}
