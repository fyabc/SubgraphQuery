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
        if (degree(i) >= k - 1) {
            result += permute(degree(i), k - 1);
        }
    }

    return result;
}

mpz_class Graph::getSubgraphNumber_Star_DegreesHard(const Graph &Q, std::size_t constraint, std::size_t constraintRoot) const {
    mpz_class result(0);

    auto k = Q.size();

    for (size_t i = 0; i < N; ++i) {
        size_t candidateNum = 0;

        size_t c1 = 0, c2 = 0;

        if (degree(i) >= k - 1 && (constraintRoot == 0 || degree(i) == constraintRoot)) {
            for (const auto &v: getAdj(i)) {
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
        size_t candidateNum = 0;
        if (degree(i) >= k - 1 && (constraintRoot == 0 || degree(i) >= constraintRoot)) {
            for (const auto &v: getAdj(i)) {
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
        size_t candidateNum = 0;
        if (degree(i) >= k - 1 && rootPred(degree(i))) {
            for (const auto &v: getAdj(i)) {
                if (leafPred(degree(v)))
                    ++candidateNum;
            }
            result += permute(candidateNum, k - 1);
        }
    }

    return result;
}

mpz_class Graph::getSubgraphNumber_StarAnd1Edge(const Graph &Q) const {
    mpz_class result(0);

    auto k = Q.size();

    for (size_t i = 0; i < N; ++i) {
        if (degree(i) <= k)
            continue;

        size_t adjEdgeNum = 0;
        for (auto u: getAdj(i)) {
            for (auto v: getAdj(u)) {
                if (u >= v)
                    continue;
                if (getAdj(i).find(v) != getAdj(i).end())
                    ++adjEdgeNum;
            }
        }

        result += permute(degree(i) - 2, k - 3) * adjEdgeNum;
    }

    return result;
}
