#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

template <typename T>
std::vector<std::vector<T>> MatrixMultiply(const std::vector<std::vector<T>>& A,
                                           const std::vector<std::vector<T>>& B) {
    assert(!A.empty() && !A[0].empty() && !B.empty() && !B[0].empty());
    assert(A[0].size() == B.size());
    std::vector<std::vector<T>> C (A.size(), std::vector<T>(B[0].size()));
    for (size_t i = 0; i < A.size(); i++) {
        for (size_t j = 0; j < B[0].size(); j++) {
            for (size_t k = 0; k < A[0].size(); k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

std::pair<std::vector<std::vector<size_t>>, std::vector<std::vector<size_t>>> MatrixChainOrder(const std::vector<size_t>& p) {
    assert(p.size() > 1);
    size_t n = p.size() - 1;
    std::vector<std::vector<size_t>> m (n, std::vector<size_t>(n));
    std::vector<std::vector<size_t>> s (n, std::vector<size_t>(n));
    for (size_t l = 2; l <= n; l++) {
        for (size_t i = 0; i < n - l + 1; i++) {
            size_t j = i + l - 1;
            m[i][j] = std::numeric_limits<size_t>::max();
            for (size_t k = i; k < j; k++) {
                auto q = m[i][k] + m[k + 1][j] + p[i] * p[k + 1] * p[j + 1];
                if (q < m[i][j]) {
                    m[i][j] = q;
                    s[i][j] = k;
                }
            }
        }
    }
    return {m, s};
}

void PrintOptimalParens(const std::vector<std::vector<size_t>>& s, size_t i, size_t j) {
    if (i == j) {
        std::cout << "A_" << i;
    } else {
        std::cout << '(';
        PrintOptimalParens(s, i, s[i][j]);
        PrintOptimalParens(s, s[i][j] + 1, j);
        std::cout << ')';
    }
}

int main() {
    std::vector<size_t> p {30, 35, 15, 5, 10, 20, 25};
    auto [m, s] = MatrixChainOrder(p);
    PrintOptimalParens(s, 0, 5);
}