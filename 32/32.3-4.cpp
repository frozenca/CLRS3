#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

constexpr std::size_t num_chars = 8;
constexpr char start_char = 'a';

std::size_t LongestCommonPrefixLength(const std::string& P1, const std::string& P2) {
    std::size_t k = 0;
    while (P1[k] == P2[k]) {
        k++;
    }
    return k;
}

bool SuffixCheck(std::string_view P_sv, std::size_t k, std::size_t q, char c) {
    return (k == 0) || (P_sv.substr(0, k - 1) == P_sv.substr(q - (k - 1), k - 1) && P_sv[k - 1] == c);
}

std::vector<std::size_t> ComputeTransitionFunction(const std::string& P1, const std::string& P2) {
    assert(!P1.empty() && !P2.empty());
    const std::size_t m_min = LongestCommonPrefixLength(P1, P2);
    const std::size_t m1 = m_min + (P1.length() - m_min);
    const std::size_t m2 = m1 + (P2.length() - m_min);
    std::string_view P1_sv (P1);
    std::string_view P2_sv (P2);
    std::vector<std::size_t> delta (num_chars * (m2 + 1));
    for (std::size_t q = 0; q <= m2; q++) {
        std::size_t q1 = 0, q2 = 0;
        if (q <= m_min) {
            q1 = q;
            q2 = q;
        } else if (q < m1) {
            q1 = q;
            q2 = m_min;
        } else {
            q1 = m_min;
            q2 = q - (m1 - m_min);
        }
        for (char c = start_char; c < start_char + num_chars; c++) {
            std::size_t k1 = std::min(P1.length() + 1, q1 + 2);
            do {
                k1--;
            } while (!SuffixCheck(P1_sv, k1, q1, c));
            std::size_t k2 = std::min(P2.length() + 1, q2 + 2);
            do {
                k2--;
            } while (!SuffixCheck(P2_sv, k2, q2, c));
            if (k2 > m_min) {
                delta[q * num_chars + c - start_char] = k2 + (m1 - m_min);
            } else {
                delta[q * num_chars + c - start_char] = k1;
            }
        }
    }
    return delta;
}

std::vector<std::size_t> FiniteAutomatonMatcher(const std::string& T, const std::vector<std::size_t>& delta,
                                                const std::size_t m_min, const std::size_t m1, const std::size_t m2) {
    assert(!T.empty() && !delta.empty() && m_min);
    const std::size_t n = T.length();
    std::vector<std::size_t> res;
    if (n < m1 && n < m_min + m2 - m1) {
        return res;
    }
    std::size_t q = 0;
    for (std::size_t i = 0; i < n; i++) {
        q = delta[q * num_chars + T[i] - start_char];
        if (q == m1) {
            res.push_back((i + 1) - m1);
        } else if (q == m2) {
            res.push_back((i + 1) - (m_min + m2 - m1));
        }
    }
    return res;
}

std::vector<std::size_t> FiniteAutomatonMatcher(const std::string& T, const std::string& P1, const std::string& P2) {
    auto delta = ComputeTransitionFunction(P1, P2);
    const std::size_t m_min = LongestCommonPrefixLength(P1, P2);
    const std::size_t m1 = m_min + (P1.length() - m_min);
    const std::size_t m2 = m1 + (P2.length() - m_min);
    return FiniteAutomatonMatcher(T, delta, m_min, m1, m2);
}

int main() {
    auto res = FiniteAutomatonMatcher("aaaaabcdcbaaabcdefghaaabcdcbaaa", "abcdefgh", "abcdcba");
    for (auto shift : res) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';

}