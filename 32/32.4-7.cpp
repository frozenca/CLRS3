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

bool isCyclic(const std::string& T, const std::string& T2) {
    if (T.length() != T2.length()) {
        return false;
    }
    std::string TT = T + T;
    return !KMPMatcher(TT, T2).empty();
}

int main() {
    assert(isCyclic("abc", "bca"));

}