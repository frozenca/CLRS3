#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

constexpr std::size_t num_chars = 26;
constexpr char start_char = 'a';

bool SuffixCheck(std::string_view P_sv, std::size_t k, std::size_t q, char c) {
    return (k == 0) || (P_sv.substr(0, k - 1) == P_sv.substr(q - (k - 1), k - 1) && P_sv[k - 1] == c);
}

std::vector<std::size_t> ComputeTransitionFunction(const std::string& P) {
    assert(!P.empty());
    const std::size_t m = P.length();
    std::string_view P_sv (P);
    std::vector<std::size_t> delta (num_chars * (m + 1));
    for (std::size_t q = 0; q <= m; q++) {
        for (char c = start_char; c < start_char + num_chars; c++) {
            std::size_t k = std::min(m + 1, q + 2);
            do {
                k--;
            } while (!SuffixCheck(P_sv, k, q, c));
            delta[q * num_chars + c - start_char] = k;
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