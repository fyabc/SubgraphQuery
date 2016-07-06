//
// Created by fanyang on 16-6-24.
//

#include "graph.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <getopt.h>

using namespace std;

struct OptionArgs {
    bool help = false;
    bool printDetails = false;
    bool generateQ = false;

    string gFileName;
    string qFileName;
    string loggerFileName;
    int sampleTimes = 1000;
};

void parseOpt(int argc, char* argv[], OptionArgs* optionArgs) {
    const char optString[] = "g:q:Q:t:l:dh";

    int opt;
    while ((opt = getopt(argc, argv, optString)) != -1) {
        switch (opt) {
            case 'g':
                optionArgs->gFileName = optarg;
                break;
            case 'q':
                optionArgs->generateQ = false;
                optionArgs->qFileName = optarg;
                break;
            case 'Q':
                optionArgs->generateQ = true;
                optionArgs->qFileName = optarg;
                break;
            case 't':
                optionArgs->sampleTimes = atoi(optarg);
                break;
            case 'l':
                optionArgs->loggerFileName = optarg;
                break;
            case 'd':
                optionArgs->printDetails = true;
                break;
            case 'h':
                optionArgs->help = true;
            default:
                break;
        }
    }
}

void usage() {
    cout <<
    "Usage\n"
    "    query -g gFile [-q qFile] [-Q type] [-t times] [-l loggerFile] [-d] [-h]\n"
    "\n"
    "Do a subgraph query.\n"
    "\n"
    "Options:\n"
    "    -g gFile        the file name of graph G\n"
    "    -q qFile        the file name of graph Q\n"
    "    -Q type         the type of graph Q (not from file)\n"
    "                    'type' string is seen below\n"
    "    -t times        the times of subgraph test (default is 10000)\n"
    "    -l loggerFile   the file name of logger (the file must exist)\n"
    "    -d              print details (default is OFF)\n"
    "    -h              print this usage and exit\n"
    "\n"
    "Details of '-Q type':\n"
    "    -Q type:arg1:arg2:...\n"
    "\n"
    "    Available types:\n"
    "        c           cycle graph, arg1 is vertex number\n"
    "        s           star graph, arg1 is vertex number\n"
    "        t           full tree graph, arg1 is depth, arg2 is branch factor\n"
    "\n"
    "    Example:\n"
    "        -Q t:2:3\n"
    << endl;
}

void printResult(const OptionArgs& optionArgs, const Graph& G, const Graph& Q, int result, ostream& out) {
    if (optionArgs.printDetails) {
        out << "Query result:\n";
        out << "    G: vertex number = " << G.size() << ", edge number = " << G.edgeNum() << "\n";
        out << "    Q: vertex number = " << Q.size() << ", edge number = " << Q.edgeNum() << "\n";
        out << "    Test " << optionArgs.sampleTimes << " times, fit " << result << " times\n";
        out << "Probability: " << double(result) / optionArgs.sampleTimes << endl;
    }
    else {
        out << result << " " << optionArgs.sampleTimes << endl;
    }
}

int main(int argc, char* argv[]) {
    OptionArgs optionArgs;
    parseOpt(argc, argv, &optionArgs);

    if (optionArgs.help) {
        usage();
        return 0;
    }

    srand((unsigned)time(nullptr));

    auto pG = Graph::fromFile(optionArgs.gFileName);

    unique_ptr<Graph> pQ;
    if (!optionArgs.generateQ)
        pQ = Graph::fromFile(optionArgs.qFileName);
    else {
        pQ = Graph::fromTypeString(optionArgs.qFileName);
    }

    auto result = pG->testSubgraph(*pQ, optionArgs.sampleTimes);

    if (optionArgs.loggerFileName != "") {
        ofstream loggerFile(optionArgs.loggerFileName.c_str(), ofstream::app);
        printResult(optionArgs, *pG, *pQ, result, loggerFile);
        loggerFile.close();
    }
    else {
        printResult(optionArgs, *pG, *pQ, result, cout);
    }

    return 0;
}