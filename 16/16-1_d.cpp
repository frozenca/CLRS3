#include <cassert>
#include <vector>
#include <utility>
#include <limits>

std::pair<std::vector<size_t>, std::vector<size_t>> computeChange(size_t n, const std::vector<size_t>& d,
                                                                  size_t k) {
    std::vector<size_t> c (n + 1);
    std::vector<size_t> denom (n + 1);
    for (size_t j = 1; j <= n; j++) {
        c[j] = std::numeric_limits<size_t>::max();
        for (size_t i = 1; i <= k; i++) {
            if (j > d[i] && 1 + c[j - d[i]] < c[j]) {
                c[j] = 1 + c[j - d[i]];
                denom[j] = d[i];
            }
        }
    }
    return {c, denom};
}

int main() {
}
