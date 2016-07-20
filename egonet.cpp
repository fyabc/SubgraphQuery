//
// Created by fanyang on 7/20/2016.
//

#include "graph.h"
#include "utils.h"

using namespace std;

Graph Graph::extractEgonet(std::size_t ego) const {
    size_t k = degree(ego) + 1;
    Graph result(k);

    const auto& egoAdj = getAdj(ego);

    unordered_map<size_t, size_t> verticesMap;

    verticesMap[ego] = 0;

    size_t i = 1;
    for (auto u: egoAdj) {
        verticesMap[u] = i;
        result.addEdge(0, i);
        ++i;
    }

    for (auto u: egoAdj) {
        for (auto v: getAdj(u)) {
            if (u > v)
                continue;
            if (egoAdj.find(v) != egoAdj.end())
                result.addEdge(verticesMap[u], verticesMap[v]);
        }
    }

    return result;
}
