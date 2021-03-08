#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <vector>

namespace sr = std::ranges;

bool SubsetSumUnary(const std::vector<std::size_t>& S, std::size_t t) {
    if (S.empty()) {
        return false;
    }
    const std::size_t n = S.size();
    assert(sr::find(S, 0) == S.end());
    std::vector<std::vector<int>> A(n, std::vector<int>(t + 1));
    A[0][0] = true;
    A[0][S[0]] = true;
    for (std::size_t i = 1; i < n; i++) {
        for (std::size_t j = 1; j <= t; j++) {
            A[i][j] = A[i - 1][j] || (j >= S[i] && A[i - 1][j - S[i]]);
        }
    }
    return A[n - 1][t];
}

int main() {
    assert(SubsetSumUnary(std::vector<std::size_t>{2, 3, 4, 5}, 8));
    assert(!SubsetSumUnary(std::vector<std::size_t>{2, 3, 4, 5}, 16));

}
