//
// Created by fanyang on 16-6-30.
//

#ifndef SUBGRAPHQUERY_UTILS_H
#define SUBGRAPHQUERY_UTILS_H

#include <gmpxx.h>
#include <cmath>
#include <iostream>

inline std::size_t getSampleTime(std::size_t k, double confidence, double error) {
    return (std::size_t)(exp(k) * log(1 / confidence) / error / error);
}

inline mpz_class fac(std::size_t n) {
    mpz_class result(1);

    for (std::size_t i = 1; i <= n; ++i)
        result *= i;

    return result;
}

inline mpz_class power(int base, std::size_t exp) {
    mpz_class result(1);

    for (std::size_t i = 1; i <= exp; ++i)
        result *= base;

    return result;
}

inline mpz_class permute(std::size_t n, std::size_t k) {
    if (n < k)
        return mpz_class(0);

    mpz_class result(1);

    for (std::size_t i = 0; i < k; ++i)
        result *= n - i;

    return result;
}

inline bool nextCombination(std::size_t n, std::size_t k, std::size_t* comb) {
    int i = 0;
    for (auto p = comb + k - 1; i < k && *p == n - i - 1; ++i, --p);

    if (i == k) {
        for (std::size_t j = 0; j < k; ++j)
            comb[j] = j;
        return false;
    }

    auto p = comb + k - i - 1;
    auto val = *p;
    for (; p != comb + k; ++p)
        *p = ++val;

    return true;
}

#endif //SUBGRAPHQUERY_UTILS_H
