//
// Created by fanyang on 16-6-30.
//

#include "graph.h"

#include <algorithm>
#include <random>
#include <cstdlib>

using namespace std;

namespace {

size_t *all = nullptr;

inline void randomSampleInit(size_t N) {
    if (all != nullptr)
        delete[] all;
    all = new size_t[N];
    for (size_t i = 0; i < N; ++i)
        all[i] = i;
    shuffle(all, all + N, default_random_engine());
}

inline void randomSample(size_t N, size_t qSize, size_t *result) {
    size_t rIndex;

    for (size_t i = 0; i < qSize; ++i) {
        rIndex = rand() % (N - i);
        result[i] = all[rIndex];

        iter_swap(all + rIndex, all + N - i - 1);
    }
}

} // anonymous namespace

bool Graph::contain(const size_t* ver, const Graph& Q) const {
    auto qSize = Q.size();

    for (size_t i = 0; i < qSize; ++i) {
        if (vertices[ver[i]].degree < Q.degree(i))
            return false;

        const auto& adjI = getAdj(ver[i]);
        for (const auto& j: Q.getAdj(i)) {
            if (adjI.find(ver[j]) == adjI.end())
                return false;
        }
    }

    return true;
}

int Graph::sampleSubgraph(const Graph &Q, int sampleTimes) const {
    int result = 0;

    auto sample = new std::size_t[Q.size()];
    randomSampleInit(N);

    for (auto i = 0; i < sampleTimes; ++i) {
        randomSample(N, Q.size(), sample);
        if (contain(sample, Q))
            ++result;
    }

    delete[] sample;

    return result;
}

int Graph::sampleSubgraph_EgonetWithoutEgo(const Graph &Q, std::size_t root, int sampleTimes) const {
    int result = 0;

    auto n = getAdj(root).size();
    auto k = Q.size();

    if (n < k)
        return 0;

    if (all != nullptr)
        delete[] all;
    all = new size_t[n];
    copy(getAdj(root).cbegin(), getAdj(root).cend(), all);
    shuffle(all, all + n, default_random_engine());

    auto sample = new size_t[n];
    for (auto i = 0; i < sampleTimes; ++i) {
        randomSample(n, k, sample);
        if (contain(sample, Q))
            ++result;
    }

    delete[] sample;
    return result;
}
