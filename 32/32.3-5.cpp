#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

constexpr std::size_t num_chars = 3;
constexpr char start_char = 'a';

bool SuffixCheck(std::string_view P_sv, std::size_t k, std::size_t q, char c) {
    return (k == 0) || (P_sv.substr(0, k - 1) == P_sv.substr(q - (k - 1), k - 1) && P_sv[k - 1] == c);
}

std::vector<std::string> Tokenize(const std::string& P, const char delimeter = ' ') {
    std::vector<std::string> result;
    std::string token;
    std::stringstream ss(P);

    while (std::getline(ss, token, delimeter)) {
        result.push_back(token);
    }
    return result;
}

std::pair<std::vector<std::size_t>, std::size_t> ComputeTransitionFunction(const std::string& P) {
    assert(!P.empty());
    auto P_ = Tokenize(P);
    std::size_t m = 0;
    for (const auto& token : P_) {
        m += token.length();
    }
    std::vector<std::size_t> delta (num_chars * (m + 1));
    std::size_t offset = 0;
    for (std::size_t i = 0; i < P_.size(); i++) {
        const auto& token = P_[i];
        std::string_view token_sv (token);
        bool is_last = (i == P_.size() - 1);
        for (std::size_t q = offset; q < offset + token.length() + is_last; q++) {
            for (char c = start_char; c < start_char + num_chars; c++) {
                std::size_t k = std::min(token.length() + 1, (q - offset) + 2);
                do {
                    k--;
                } while (!SuffixCheck(token_sv, k, q - offset, c));
                delta[q * num_chars + c - start_char] = k + offset;
            }
        }
        offset += token.length();
    }
    return {delta, m};
}

bool FiniteAutomatonMatcher(const std::string& T, const std::vector<std::size_t>& delta, std::size_t m) {
    assert(!T.empty() && !delta.empty() && m);
    const std::size_t n = T.length();
    std::size_t q = 0;
    for (std::size_t i = 0; i < n; i++) {
        q = delta[q * num_chars + T[i] - start_char];
        if (q == m) {
            return true;
        }
    }
    return false;
}

bool FiniteAutomatonMatcher(const std::string& T, const std::string& P) {
    auto [delta, m] = ComputeTransitionFunction(P);
    return FiniteAutomatonMatcher(T, delta, m);
}

int main() {
    assert(FiniteAutomatonMatcher("cabccbacbacab", "ab ba c"));
}