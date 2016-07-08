//
// Created by fanyang on 16-6-30.
//

#ifndef SUBGRAPHQUERY_UTILS_H
#define SUBGRAPHQUERY_UTILS_H

#include <gmpxx.h>
#include <cmath>
#include <iostream>
#include <unordered_map>

inline std::size_t urand() {
    return (std::size_t)rand();
}

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

struct PairHash {
    std::size_t operator() (const std::pair<std::size_t, std::size_t>& val) const {
        return std::hash<std::size_t>()(val.first) ^ (std::hash<std::size_t>()(val.second) << 1);
    }
};

inline mpz_class permute(std::size_t n, std::size_t k) {
    using namespace std;
    static unordered_map<pair<size_t, size_t>, mpz_class, PairHash> calculated;

    if (n < k)
        return mpz_class(0);

    auto pResult = calculated.find(pair<size_t, size_t>(n, k));
    if (pResult != calculated.end())
        return pResult->second;

    mpz_class result(1);

    for (size_t i = 0; i < k; ++i)
        result *= n - i;

    calculated[pair<size_t, size_t>(n, k)] = result;

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

inline void expRep(const mpz_class& val, std::ostream& out = std::cout) {
    auto rep = val.get_str(10);
    auto exp = int(rep.size() - 1);
    out << rep[0] << '.' << rep.substr(1, (std::size_t)std::min(2, exp)) << "E" << exp;
}

#endif //SUBGRAPHQUERY_UTILS_H
