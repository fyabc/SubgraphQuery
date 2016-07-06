//
// Created by fanyang on 7/6/2016.
//

#include "graph.h"
#include "utils.h"

using namespace std;

namespace {

} // anonymous namespace

mpz_class Graph::getSubgraphNumber_Star(const Graph &Q) const {
    mpz_class result(0);

    auto k = Q.size();

    for (size_t i = 0; i < N; ++i) {
        const auto& vertexI = vertices[i];
        if (vertexI.degree >= k - 1) {
            result += permute(vertexI.degree, k - 1);
        }
    }

    return result;
}