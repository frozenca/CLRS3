#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

size_t BreakString(size_t n, std::vector<size_t>& B) {
    size_t m = B.size();
    std::vector<size_t> B_ (m + 2);
    B_[m + 1] = n;
    for (size_t i = 1; i <= m; i++) {
        B_[i] = B[i - 1];
    }
    B = B_;
    std::vector<std::vector<size_t>> C (m + 1, std::vector<size_t> (m + 2));
    for (size_t i = m - 1; i < m; i--) {
        for (size_t j = i + 2; j <= m + 1; j++) {
            C[i][j] = std::numeric_limits<size_t>::max();
            for (size_t k = i + 1; k <= j - 1; k++) {
                C[i][j] = std::min({C[i][j], C[i][k] + C[k][j] + B[j] - B[i]});
            }
        }
    }
    return C[0][m + 1];
}

int main() {
    std::vector<size_t> B {2, 8, 10};
    std::cout << BreakString(20, B);

}