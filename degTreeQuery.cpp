//
// Created by fanyang on 16-7-1.
//

#include "graph.h"

using namespace std;


mpz_class Graph::getSubgraphNumber_Tree_DegreesHard(const Graph& Q, const vector<int>& constraints) const {
    mpz_class result(0);
    auto k = Q.size();

    auto degreeDistribution = getDegreeDistribution();

    for (size_t i = 0; i < k; ++i) {
        auto consI = constraints[i];

        if (consI != -1 && degreeDistribution[consI].empty())
            return mpz_class(0);
    }

    return result;
}

mpz_class Graph::getSubgraphNumber_Tree_DegreesSoft(const Graph& Q, const vector<int>& constraints) const {
    mpz_class result(0);

    auto degreeDistribution = getDegreeDistribution();

    return result;
}