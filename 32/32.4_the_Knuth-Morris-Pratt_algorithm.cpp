#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

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

std::vector<std::size_t> KMPMatcher(const std::string& T, const std::string& P) {
    const std::size_t n = T.length();
    const std::size_t m = P.length();
    std::vector<std::size_t> res;
    if (n < m) {
        return res;
    }
    auto pi = ComputePrefixFunction(P);
    std::size_t q = 0;
    for (std::size_t i = 0; i < n; i++) {
        while (q && P[q] != T[i]) {
            q = pi[q];
        }
        if (P[q] == T[i]) {
            q++;
        }
        if (q == m) {
            res.push_back((i + 1) - m);
            q = pi[q];
        }
    }
    return res;
}

int main() {
    auto res = KMPMatcher("acaabc", "aab");
    for (auto shift : res) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res2 = KMPMatcher("acaabc", "abc");
    for (auto shift : res2) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res3 = KMPMatcher("acaabc", "aca");
    for (auto shift : res3) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res4 = KMPMatcher("acaabc", "a");
    for (auto shift : res4) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
}