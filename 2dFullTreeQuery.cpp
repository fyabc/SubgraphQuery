//
// Created by fanyang on 16-7-6.
//

#include "graph.h"
#include "utils.h"

#include <random>

using namespace std;

namespace {

void randomSetSample(const unordered_set<size_t>& adj, size_t k, size_t omitted, size_t* result) {
    vector<size_t> vAdj;

    for (const auto& v: adj)
        if (v != omitted)
            vAdj.push_back(v);

    shuffle(vAdj.begin(), vAdj.end(), default_random_engine());
    copy(vAdj.begin(), vAdj.begin() + k, result);
}

void randomSetSample(const unordered_set<size_t>& adj, size_t k, size_t* result) {
    vector<size_t> vAdj;

    for (const auto& v: adj)
        vAdj.push_back(v);

    shuffle(vAdj.begin(), vAdj.end(), default_random_engine());
    copy(vAdj.begin(), vAdj.begin() + k, result);
}

} // anonymous namespace

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

double Graph::testSubgraph_2dTree(std::size_t b1, std::size_t b2, int sampleTimes) const {
    int result = 0;
    auto sample1 = new size_t[b1];
    auto sample2 = new size_t[b2];
    vector<bool> used(N, false);

    vector<size_t> candidateRoots;
    for (size_t i = 0; i < N; ++i)
        if (degree(i) >= b1)
            candidateRoots.push_back(i);

    for (int i = 1; i <= sampleTimes;) {
        used.assign(N, false);

        // choose root.
        auto root = candidateRoots[rand() % candidateRoots.size()];
        used[root] = true;

        // choose layer 1.
        unordered_set<size_t> candidateL1s;
        for (const auto& elem: vertices[root].adj)
            if (degree(elem) > b2)
                candidateL1s.insert(elem);

        if (candidateL1s.size() < b1)
            continue;

        randomSetSample(candidateL1s, b1, sample1);
        for (auto p = sample1; p != sample1 + b1; ++p)
            used[*p] = true;

        // choose layer 2.
        auto p = sample1;
        for (; p != sample1 + b1; ++p) {
            randomSetSample(vertices[*p].adj, b2, root, sample2);
            for (auto q = sample2; q != sample2 + b2; ++q) {
                if (used[*q])
                    goto endLoop;
                used[*q] = true;
            }
        }

        endLoop:
        if (p == sample1 + b1) {
            ++result;
//            cout << "Success! Rate = " << double(result) / i << endl;
        }
        else {
//            cout << "Fail!    Rate = " << double(result) / i << endl;
        }

        ++i;
    }

    delete[] sample1;
    delete[] sample2;

    return double(result) / sampleTimes;
}