#include <bit>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>

std::size_t ModularExponentiation(std::size_t a, std::size_t b, std::size_t n) {
    assert(n);
    std::size_t c = 0;
    std::size_t d = 1;
    auto k = std::bit_width(b);
    std::bitset<64> B(b);
    for (std::size_t i = k - 1; i < k; i--) {
        c *= 2;
        d = (d * d) % n;
        if (B[i]) {
            c++;
            d = (d * a) % n;
        }
    }
    return d;
}

// we don't use radix as 128. it will easily overflow and wraparound
constexpr std::size_t num_chars = 26;
constexpr char start_char = 'a';

std::vector<std::pair<std::size_t, std::size_t>> RabinKarpMultipleMatcher(const std::string& T, const std::vector<std::string>& patterns,
                                          const std::size_t d, const std::size_t q) {
    assert(!T.empty() && !patterns.empty());
    const std::size_t n = T.length();
    const std::size_t k = patterns.size();

    std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>> rolling_hashes;

    std::size_t m = std::numeric_limits<std::size_t>::max();

    for (std::size_t j = 0; j < k; j++) {
        const auto& pattern = patterns[j];
        m = std::min(m, pattern.length());
    }
    if (n < m) {
        return {};
    }

    std::size_t h = ModularExponentiation(d, m - 1, q);

    for (std::size_t j = 0; j < k; j++) {
        const auto& pattern = patterns[j];
        std::size_t curr_hash = 0;
        for (std::size_t l = 0; l < m; l++) {
            curr_hash = (d * curr_hash + pattern[l] - start_char) % q;
        }
        std::size_t offset = 0;
        rolling_hashes[curr_hash].emplace_back(offset, j);
        for (offset = 1; offset <= pattern.length() - m; offset++) {
            curr_hash = ((d * (curr_hash + q - ((pattern[offset - 1] - start_char) * h) % q)) +
                         (pattern[offset + m - 1] - start_char)) % q;
            rolling_hashes[curr_hash].emplace_back(offset, j);
        }
    }

    std::string_view text_view (T);
    std::vector<std::string_view> pattern_views(k);
    for (std::size_t j = 0; j < k; j++) {
        pattern_views[j] = std::string_view(patterns[j]);
    }
    std::size_t text_curr_hash = 0;
    for (std::size_t l = 0; l < m; l++) {
        text_curr_hash = (d * text_curr_hash + T[l] - start_char) % q;
    }

    std::vector<std::unordered_set<std::size_t>> already_checked (k);
    std::vector<std::pair<std::size_t, std::size_t>> res;
    for (std::size_t s = 0; s <= n - m; s++) {
        const auto& curr_candidates = rolling_hashes[text_curr_hash];
        for (const auto& [offset, index] : curr_candidates) {
            std::size_t left = s - offset;
            std::size_t right = left + patterns[index].length();
            std::size_t pair_index = left * n + right;
            if (!already_checked[index].contains(pair_index)) {
                already_checked[index].insert(pair_index);
                if (text_view.substr(left, patterns[index].length()) == pattern_views[index]) {
                    res.emplace_back(left, index);
                }
            }
        }
        if (s < n - m) {
            text_curr_hash = (d * (text_curr_hash + q - ((text_view[s] - start_char) * h) % q) + (text_view[s + m] - start_char)) % q;
        }
    }
    return res;
}

int main() {
    std::vector<std::string> patterns {"bac", "cbacd", "dacb", "bb"};
    auto res = RabinKarpMultipleMatcher("ababacbacdacbdbbcd", patterns, num_chars, 11);
    for (const auto& [shift, index] : res) {
        std::cout << shift << ' ' << index << '\n';
    }
}