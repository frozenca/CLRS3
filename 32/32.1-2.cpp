#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::size_t> DistinctNaiveMatcher(const std::string& T, const std::string& P) {
    assert(!T.empty() && !P.empty());
    const std::size_t n = T.length();
    const std::size_t m = P.length();
    std::vector<std::size_t> res;
    if (n < m) {
        return res;
    }
    std::size_t k = 0;
    for (std::size_t s = 0; s < n; s++) {
        if (T[s] == P[k]) {
            k++;
        } else if (T[s] == P[0]) {
            k = 1;
        } else {
            k = 0;
        }
        if (k == m) {
            res.push_back(s + 1 - m);
            k = 0;
        }
    }
    return res;
}

int main() {
    auto res = DistinctNaiveMatcher("acaabc", "abcd");
    for (auto shift : res) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res2 = DistinctNaiveMatcher("acaabc", "ac");
    for (auto shift : res2) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res3 = DistinctNaiveMatcher("acaabc", "ab");
    for (auto shift : res3) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
    auto res4 = DistinctNaiveMatcher("acaabc", "a");
    for (auto shift : res4) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
}