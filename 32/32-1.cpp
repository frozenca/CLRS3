#include <cassert>
#include <cstddef>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ranges>

namespace sr = std::ranges;

std::vector<std::size_t> ComputePrefixFunction(const std::string& P) {
    assert(!P.empty());
    const std::size_t m = P.length();
    std::vector<std::size_t> pi(m + 1);
    std::size_t k = 0;
    for (std::size_t q = 2; q <= m; q++) {
        while (k && P[k] != P[q - 1]) {
            k = pi[k];
        }
        if (P[k] == P[q - 1]) {
            k++;
        }
        pi[q] = k;
    }
    return pi;
}

std::vector<std::size_t> LongestRepetition(const std::string& P) {
    assert(!P.empty());
    const std::size_t m = P.length();
    auto pi = ComputePrefixFunction(P);
    std::vector<std::size_t> rho(m + 1);
    for (std::size_t i = 1; i <= m; i++) {
        std::size_t k = i - pi[i];
        if (i % k == 0) {
            rho[i] = i / k;
        } else {
            rho[i] = 1;
        }
    }
    return rho;
}

std::vector<std::size_t> RepetitionMatcher(const std::string& T, const std::string& P) {
    const std::size_t m = P.length();
    const std::size_t n = T.length();
    if (n < m) {
        return {};
    }
    auto rho = LongestRepetition(P);
    auto rho_P = *sr::max_element(rho);
    std::size_t k = 1 + rho_P;
    std::size_t q = 0;
    std::size_t s = 0;
    std::vector<std::size_t> res;
    while (s <= n - m) {
        if (T[s + q] == P[q]) {
            q++;
            if (q == m) {
                res.push_back(s);
            }
        }
        if (q == m || T[s + q] != P[q]) {
            s += std::max(1, static_cast<int>(std::ceil(q / k)));
            q = 0;
        }
    }
    return res;
}

int main() {
    auto res = RepetitionMatcher("acaabc", "aab");
    for (auto shift : res) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res2 = RepetitionMatcher("acaabc", "abc");
    for (auto shift : res2) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res3 = RepetitionMatcher("acaabc", "aca");
    for (auto shift : res3) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res4 = RepetitionMatcher("acaabc", "a");
    for (auto shift : res4) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
}