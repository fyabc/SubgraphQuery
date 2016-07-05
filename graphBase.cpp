//
// Created by fanyang on 16-6-30.
//

#include "graph.h"

#include <fstream>
#include <algorithm>
#include <queue>
#include <unordered_set>

using namespace std;

namespace {

inline void parseFileName(const string& fileName, size_t& n, double& p) {
    // only parse after last slash.
    auto lastSlash = find(fileName.crbegin(), fileName.crend(), '/');
    auto realFileName = fileName.substr(size_t(fileName.crend() - lastSlash));

    // erase ".txt"
    realFileName.erase(realFileName.size() - 4);

    auto firstUnderline = find(realFileName.cbegin(), realFileName.cend(), '_');
    auto firstUnderlineIndex = firstUnderline - realFileName.cbegin();
//    auto graphType = realFileName.substr(0, size_t(firstUnderlineIndex));
    auto secondUnderline = find(firstUnderline + 1, realFileName.cend(), '_');
    auto secondUnderlineIndex = secondUnderline - realFileName.cbegin();
    n = (size_t)atoi(realFileName.substr(firstUnderlineIndex + 1, secondUnderlineIndex - firstUnderlineIndex - 1).c_str());
    p = atof(realFileName.substr(secondUnderlineIndex + 1).c_str());
}

} // anonymous namespace

size_t Graph::edgeNum() const {
    size_t result = 0;

    for (const auto& elem : vertices)
        result += elem.degree;

    return result / 2;
}

unique_ptr<Graph> Graph::fromFile(const string& fileName) {
    size_t n = 10;
    double p = 0.0;

    try {
        parseFileName(fileName, n, p);
    }
    catch (exception& e) {
        throw runtime_error("Error when parsing file name");
    }

    auto result = unique_ptr<Graph>(new Graph(n));

    ifstream inputFile(fileName, ifstream::in);
    if (!inputFile.good()) {
        throw runtime_error("Cannot open such file");
    }

    // read and omit first 4 comment lines.
    string line;
    for (auto i = 0; i < 4; ++i) {
        getline(inputFile, line);
    }

    std::size_t src, dst;
    while (inputFile >> src >> dst) {
        result->addEdge(src, dst);
    }

    inputFile.close();

    return result;
}

std::unique_ptr<Graph> Graph::fromTypeString(const std::string& typeString) {
    auto firstColonIndex = find(typeString.cbegin(), typeString.cend(), ':') - typeString.cbegin();
    auto type = typeString.substr(0, firstColonIndex);
    size_t arg1, arg2;

    switch (type[0]) {
        case 'c':
            arg1 = (size_t)atoi(typeString.substr(firstColonIndex + 1).c_str());
            return createCycle(arg1);
        case 's':
            arg1 = (size_t)atoi(typeString.substr(firstColonIndex + 1).c_str());
            return createStar(arg1);
        case 't': {
            auto secondColonIndex = find(typeString.cbegin() + firstColonIndex + 1, typeString.cend(), ':') - typeString.cbegin();
            arg1 = (size_t)atoi(typeString.substr(firstColonIndex + 1, secondColonIndex - firstColonIndex - 1).c_str());
            arg2 = (size_t)atoi(typeString.substr(secondColonIndex + 1).c_str());
            return createTree(arg1, arg2);
        }
        default:
            throw runtime_error("unknown graph type");
    }
}

unique_ptr<Graph> Graph::createStar(std::size_t n) {
    auto result = unique_ptr<Graph>(new Graph(n));

    for (std::size_t i = 1; i < n; ++i)
        result->addEdge(0, i);

    return result;
}

unique_ptr<Graph> Graph::createCycle(std::size_t n) {
    auto result = unique_ptr<Graph>(new Graph(n));

    for (std::size_t i = 0; i < n - 1; ++i)
        result->addEdge(i, i + 1);
    result->addEdge(n - 1, 0);

    return result;
}

std::unique_ptr<Graph> Graph::createTree(std::size_t depth, std::size_t width) {
    std::size_t temp = 1, n = 1;
    for (auto i = 0; i < depth; ++i) {
        temp *= width;
        n += temp;
    }
    auto result = unique_ptr<Graph>(new Graph(n));

    for (size_t i = 0; (i + 1) * width < n; ++i)
        for (size_t j = i * width + 1; j <= (i + 1) * width; ++j)
            result->addEdge(i, j);

    return result;
}

std::unique_ptr<Graph> Graph::createTreeH(std::initializer_list<std::size_t> widths) {
    std::size_t temp = 1;
    auto depth = widths.size();
    auto n = new size_t[depth + 1];
    n[0] = 1;

    int i = 1;
    for (auto it = widths.begin(); it != widths.end(); ++it, ++i) {
        temp *= *it;
        n[i] = n[i - 1] + temp;
    }

    auto result = unique_ptr<Graph>(new Graph(n[depth]));

    auto it = widths.begin();
    for (i = 0; i < depth; ++i, ++it) {
        auto begin = (i == 0) ? 0 : n[i - 1];
        for (size_t src = begin; src < n[i]; ++src) {
            for (size_t j = 0; j < *it; ++j) {
                result->addEdge(src, n[i] + (src - begin) * (*it) + j);
            }
        }
    }

    delete[] n;

    return result;
}

void Graph::addEdge(std::size_t src, std::size_t dst) {
    vertices[src].addEdge(dst);
    vertices[dst].addEdge(src);
}

void Graph::removeEdge(std::size_t src, std::size_t dst) {
    vertices[src].removeEdge(dst);
    vertices[dst].removeEdge(src);
}

void Graph::showAdj(ostream& out) const {
    out << "\t#";
    for (auto i = 0; i < N; ++i)
        out << "\t" << i;
    out << endl;

    vector<int> edges(size_t(N), 0);
    for (auto i = 0; i < N; ++i) {
        out << "\t" << i;

        for (auto j = 0; j < N; ++j)
            edges[j] = 0;

        for (const auto& elem : vertices[i].adj) {
            edges[elem] = 1;
        }

        for (auto j = 0; j < N; ++j)
            out << "\t" << edges[j];

        out << endl;
    }
}

unordered_set<size_t> Graph::getConnectComponent(size_t start) const {
    unordered_set<size_t> result;
    auto visited = new bool[N];
    memset(visited, false, N * sizeof(bool));

    visited[start] = true;

    queue<size_t> q;
    q.push(start);

    while (!q.empty()) {
        auto ver = q.front();
        result.insert(ver);
        q.pop();

        for (const auto& elem: vertices[ver].adj) {
            if (!visited[elem]) {
                q.push(elem);
                visited[elem] = true;
            }
        }
    }

    delete[] visited;
    return result;
}

vector<unordered_set<size_t>> Graph::getDegreeDistribution() const {
    vector<unordered_set<size_t>> result(N, unordered_set<size_t>());

    for (size_t i = 0; i < N; ++i)
        result[vertices[i].degree].insert(i);

    return result;
}