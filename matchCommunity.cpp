//
// Created by fanyang on 9/20/2016.
//

#include "graph.h"

using namespace std;


mpz_class Graph::getMC_Star(const Graph &Q) const {
    return getMC_Star(Q.size());
}

mpz_class Graph::getMC_Star(std::size_t k) const {
    mpz_class result(0);

    return result;
}