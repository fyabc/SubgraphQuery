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

mpz_class Graph::getSubgraphNumber_Star_DegreesHard(const Graph &Q, std::size_t constraint, std::size_t constraintRoot) const {
    mpz_class result(0);

    auto k = Q.size();

    for (size_t i = 0; i < N; ++i) {
        const auto& vertexI = vertices[i];
        size_t candidateNum = 0;

        size_t c1 = 0, c2 = 0;

        if (vertexI.degree >= k - 1 && (constraintRoot == 0 || vertexI.degree == constraintRoot)) {
            for (const auto &v: vertexI.adj) {
                if (degree(v) == constraint)
                    ++candidateNum;
            }
            result += permute(candidateNum, k - 1);
        }
    }

    return result;
}

mpz_class Graph::getSubgraphNumber_Star_DegreesSoft(const Graph& Q, std::size_t constraint, std::size_t constraintRoot) const {
    mpz_class result(0);

    auto k = Q.size();

    for (size_t i = 0; i < N; ++i) {
        const auto& vertexI = vertices[i];
        size_t candidateNum = 0;
        if (vertexI.degree >= k - 1 && (constraintRoot == 0 || vertexI.degree >= constraintRoot)) {
            for (const auto &v: vertexI.adj) {
                if (degree(v) >= constraint)
                    ++candidateNum;
            }
            result += permute(candidateNum, k - 1);
        }
    }

    return result;
}

mpz_class Graph::getSubgraphNumber_Star_All(std::size_t k, std::size_t constraintRoot, bool rootHard,
                                            std::size_t constraintLeaf, bool leafHard) const {
    mpz_class result(0);

    auto rootPred = [constraintRoot, rootHard] (size_t deg) { return rootHard ? deg == constraintRoot : deg >= constraintRoot; };
    auto leafPred = [constraintLeaf, leafHard] (size_t deg) { return leafHard ? deg == constraintLeaf : deg >= constraintLeaf; };

    for (size_t i = 0; i < N; ++i) {
        const auto& vertexI = vertices[i];
        size_t candidateNum = 0;
        if (vertexI.degree >= k - 1 && rootPred(vertexI.degree)) {
            for (const auto &v: vertexI.adj) {
                if (leafPred(degree(v)))
                    ++candidateNum;
            }
            result += permute(candidateNum, k - 1);
        }
    }

    return result;
}