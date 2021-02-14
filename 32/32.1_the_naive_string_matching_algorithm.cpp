#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::size_t> NaiveStringMatcher(const std::string& T, const std::string& P) {
    assert(!T.empty() && !P.empty());
    const std::size_t n = T.length();
    const std::size_t m = P.length();
    std::vector<std::size_t> res;
    if (n < m) {
        return res;
    }
    std::string_view T_sv (T);
    std::string_view P_sv (P);
    for (std::size_t s = 0; s <= n - m; s++) {
        if (P_sv.substr(0, m) == T_sv.substr(s, m)) {
            res.push_back(s);
        }
    }
    return res;
}

int main() {
    auto res = NaiveStringMatcher("acaabc", "aab");
    for (auto shift : res) {
        std::cout << shift << ' ';
    }
    std::cout << '\n';
}