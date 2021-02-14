#include <bit>
#include <bitset>
#include <cmath>
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

// we don't use radix as 128. it will easily overflow and wraparound
constexpr std::size_t num_chars = 26;
constexpr char start_char = 'a';

std::vector<std::pair<std::size_t, std::size_t>> RabinKarpMatcher(const std::string& T, const std::string& P, std::size_t d, std::size_t q) {
    assert(!T.empty() && !P.empty());
    const std::size_t n = std::floor(std::sqrt(T.length()));
    const std::size_t m = std::floor(std::sqrt(P.length()));
    assert(n * n == T.length() && m * m == P.length());
    std::vector<std::pair<std::size_t, std::size_t>> res;
    if (n < m) {
        return res;
    }
    std::string_view T_sv (T);
    std::string_view P_sv (P);

    std::size_t p = 0;
    std::size_t t = 0;
    for (std::size_t i = 0; i < m * m; i++) { // preprocessing
        p = (d * p + (P_sv[i] - start_char)) % q;
        t = (d * t + (T_sv[(i / m) * n + (i % m)] - start_char)) % q;
    }

    std::size_t h_r_base = ModularExponentiation(d, m, q);
    std::vector<std::size_t> H_R (m);
    for (std::size_t r = 0; r < m; r++) {
        H_R[r] = ModularExponentiation(d, (m - 1 - r) * m, q);
    }
    std::size_t h_c_base = ModularExponentiation(d, m * m, q);
    std::vector<std::size_t> H_C (m);
    for (std::size_t c = 0; c < m; c++) {
        H_C[c] = ModularExponentiation(d, (m - 1 - c), q);
    }

    for (std::size_t r = 0; r <= n - m; r++) { // matching
        std::size_t curr_t = t;
        for (std::size_t c = 0; c <= n - m; c++) {
            if (p == curr_t) {
                bool matched = true;
                for (std::size_t R = 0; R < m; R++) {
                    if (P_sv.substr(R * m, m) != T_sv.substr((R + r) * n + c, m)) {
                        matched = false;
                    }
                }
                if (matched) {
                    res.emplace_back(r, c);
                }
            }
            if (c < n - m) {
                curr_t = (d * curr_t) % q;
                for (std::size_t R = 0; R < m; R++) {
                    curr_t = (curr_t + q - ((T_sv[(R + r) * n + c] - start_char) * H_R[R] * h_r_base) % q
                            + (T_sv[(R + r) * n + (c + m)] - start_char) * H_R[R]) % q;
                }
            }
        }
        if (r < n - m) {
            t = (h_r_base * t) % q;
            for (std::size_t C = 0; C < m; C++) {
                t = (t + q - ((T_sv[r * n + C] - start_char) * H_C[C] * h_c_base) % q
                          + (T_sv[(r + m) * n + C] - start_char) * H_C[C]) % q;
            }
        }
    }
    return res;
}

int main() {
    auto res = RabinKarpMatcher("abcdcabddcabcdca", "abca", num_chars, 11);
    for (const auto& [x, y] : res) {
        std::cout << x << ' ' << y << '\n';
    }
    std::cout << '\n';
}