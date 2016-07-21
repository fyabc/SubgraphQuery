//
// Created by fanyang on 7/20/2016.
//

#include "graph.h"
#include "utils.h"

#include <random>

using namespace std;

unique_ptr<Graph> Graph::extractEgonet(std::size_t ego, double chooseRate, bool containEgo) const {
    size_t k = degree(ego) + 1;
    unique_ptr<Graph> result(new Graph(containEgo ? k : k - 1));

    uniform_real_distribution<double> distribution(0.0, 1.0);
    default_random_engine engine;

    const auto& egoAdj = getAdj(ego);

    unordered_map<size_t, size_t> verticesMap;

    size_t currentIndex;

    if (containEgo) {
        verticesMap[ego] = 0;
        currentIndex = 1;
    }
    else {
        currentIndex = 0;
    }

    for (auto u: egoAdj) {
        verticesMap[u] = currentIndex;
        if (containEgo)
            result->addEdge(0, currentIndex);
        ++currentIndex;
    }

    for (auto u: egoAdj) {
        for (auto v: getAdj(u)) {
            if (u > v)
                continue;
            if (egoAdj.find(v) != egoAdj.end()) {
                if (distribution(engine) <= chooseRate)
                    result->addEdge(verticesMap[u], verticesMap[v]);
            }
        }
    }

    return result;
}

void helpSplit(size_t u, Graph& Q, vector<Graph::DecomposeTree2Node> &decompose, vector<bool>& visited, bool first = false) {
    visited[u] = true;

    Graph::DecomposeTree2Node node;
    auto nextIndex = decompose.size();

    if (Q.degree(u) == 0) {
        node.vertices = {0, u};
        node.children = {};
        node.annotatedVertices = {-1, -1};
        node.annotatedEdges = {-1, -1};
        node.bNodeIndexes = {0};

        // it annotate previous root node.
        for (auto it = decompose.rbegin(); it != decompose.rend(); ++it) {
            if (it->bNodeIndexes.size() == 1) {
                it->children.push_back(nextIndex);
                it->annotatedVertices[0] = nextIndex;

                break;
            }
        }
        decompose.push_back(std::move(node));

        return;
    }

    auto v = *(Q.getAdj(u).begin());
    Q.removeEdge(u, v);


    node.vertices = {0, u, v};
    node.children = {};
    node.annotatedVertices = {-1, -1, -1};
    node.annotatedEdges = {-1, -1, -1};
    node.bNodeIndexes = {0, 1};

    if (first) {
        node.bNodeIndexes = {0};

        // it annotates previous root node.
        for (auto it = decompose.rbegin(); it != decompose.rend(); ++it) {
            if (it->bNodeIndexes.size() == 1) {
                it->children.push_back(nextIndex);
                it->annotatedVertices[0] = nextIndex;

                break;
            }
        }
    }
    else {
        node.bNodeIndexes = {0, 1};

        // it annotates previous node which contains (0, u).
        for (auto it = decompose.rbegin(); it != decompose.rend(); ++it) {
            if (it->vertices.size() == 3 && (it->vertices[1] == u || it->vertices[2] == u)) {
                it->children.push_back(nextIndex);
                if (it->vertices[1] == u)
                    it->annotatedEdges[0] = nextIndex;
                else
                    it->annotatedEdges[2] = nextIndex;

                break;
            }
        }
    }

    decompose.push_back(node);

    if (Q.degree(u) > 0)
        helpSplit(u, Q, decompose, visited);

    if (Q.degree(v) > 0)
        helpSplit(v, Q, decompose, visited);
    else
        visited[v] = true;
}

bool Graph::decomposeEgonet(std::vector<Graph::DecomposeTree2Node> &decompose) const {
    if (haveK4())
        return false;

    decompose.clear();

    auto copy = *this;
    for (size_t i = 1; i < N; ++i)
        copy.removeEdge(0, i);

    vector<bool> visited(N, false);

    for (size_t u = 1; u < N; ++u) {
        if (visited[u])
            continue;

        helpSplit(u, copy, decompose, visited, true);
    }

    return true;
}
