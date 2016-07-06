//
// Created by fanyang on 16-7-6.
//

#include "graph.h"
#include "utils.h"

using namespace std;

mpz_class Graph::getSubgraphNumber_2dFullTree_ignore(size_t b1, size_t b2) const {
    mpz_class result(0);

    vector<mpz_class> numbers(N);
    for (size_t i = 0; i < N; ++i) {
        numbers[i] = permute(degree(i) - 1, b2);
    }

    size_t* comb = new size_t[b1];

    for (size_t i = 0; i < N; ++i) {
        if (degree(i) < b1)
            continue;

        vector<size_t> adj;
        for (const auto& v: vertices[i].adj)
            if (degree(v) >= b2 - 1)
                adj.push_back(v);

        auto n = adj.size();
        if (n < b1)
            continue;

        for (size_t j = 0; j < b1; ++j)
            comb[j] = j;

        do {
            mpz_class temp(1);
            for (int j = 0; j != b1; ++j)
                temp *= numbers[adj[comb[j]]];
            result += temp;
        } while (nextCombination(n, b1, comb));
    }

    delete[] comb;

    return result * fac(b1);
}