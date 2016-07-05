//
// Created by fanyang on 16-6-30.
//

#ifndef SUBGRAPHQUERY_UTILS_H
#define SUBGRAPHQUERY_UTILS_H

#include <cmath>

inline std::size_t getSampleTime(std::size_t k, double confidence, double error) {
    return (std::size_t)(exp(k) * log(1 / confidence) / error / error);
}

#endif //SUBGRAPHQUERY_UTILS_H
