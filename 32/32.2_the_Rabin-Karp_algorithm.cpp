#include <bit>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

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

std::vector<std::size_t> RabinKarpMatcher(const std::string& T, const std::string& P, std::size_t d, std::size_t q) {
    assert(!T.empty() && !P.empty());
    const std::size_t n = T.length();
    const std::size_t m = P.length();
    std::vector<std::size_t> res;
    if (n < m) {
        return res;
    }
    std::string_view T_sv (T);
    std::string_view P_sv (P);

    std::size_t h = ModularExponentiation(d, m - 1, q);
    std::size_t p = 0;
    std::size_t t = 0;

    for (std::size_t i = 0; i < m; i++) { // preprocessing
        p = (d * p + (P_sv[i] - 'a')) % q;
        t = (d * t + (T_sv[i] - 'a')) % q;
    }
    for (std::size_t s = 0; s <= n - m; s++) { // matching
        if (p == t && P_sv == T_sv.substr(s, m)) {
            res.push_back(s);
        }
        if (s < n - m) {
            t = (d * (t + q - ((T_sv[s] - 'a') * h) % q) + (T_sv[s + m] - 'a')) % q;
        }
    }
    return res;
}

// we don't use radix as 128. it will easily overflow and wraparound
constexpr std::size_t num_chars = 26;

int main() {
    auto res = RabinKarpMatcher("acaabc", "aab", num_chars, 11);
    for (auto shift : res) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
}