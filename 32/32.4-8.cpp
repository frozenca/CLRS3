#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

constexpr std::size_t num_chars = 26;
constexpr char start_char = 'a';

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

std::vector<std::size_t> ComputeTransitionFunction(const std::string& P) {
    assert(!P.empty());
    auto pi = ComputePrefixFunction(P);
    const std::size_t m = P.length();
    std::vector<std::size_t> delta (num_chars * (m + 1));
    for (std::size_t q = 0; q <= m; q++) {
        for (char c = start_char; c < start_char + num_chars; c++) {
            if (q == 0) {
                delta[q * num_chars + c - start_char] = (c == P[0]);
            } else if (q == m || P[q] != c) {
                delta[q * num_chars + c - start_char] = delta[pi[q] * num_chars + c - start_char];
            } else {
                delta[q * num_chars + c - start_char] = q + 1;
            }
        }
    }
    return delta;
}

std::vector<std::size_t> FiniteAutomatonMatcher(const std::string& T, const std::vector<std::size_t>& delta, std::size_t m) {
    assert(!T.empty() && !delta.empty() && m);
    const std::size_t n = T.length();
    std::vector<std::size_t> res;
    if (n < m) {
        return res;
    }
    std::size_t q = 0;
    for (std::size_t i = 0; i < n; i++) {
        q = delta[q * num_chars + T[i] - start_char];
        if (q == m) {
            res.push_back((i + 1) - m);
        }
    }
    return res;
}

std::vector<std::size_t> FiniteAutomatonMatcher(const std::string& T, const std::string& P) {
    auto delta = ComputeTransitionFunction(P);

    const std::size_t m = P.length();
    return FiniteAutomatonMatcher(T, delta, m);
}

int main() {
    auto res = FiniteAutomatonMatcher("acaabc", "aab");
    for (auto shift : res) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res2 = FiniteAutomatonMatcher("acaabc", "abc");
    for (auto shift : res2) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res3 = FiniteAutomatonMatcher("acaabc", "aca");
    for (auto shift : res3) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res4 = FiniteAutomatonMatcher("acaabc", "a");
    for (auto shift : res4) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
}