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

size_t RecursiveMatrixChain(const std::vector<size_t>& p, size_t i, size_t j) {
    if (i == j) {
        return 0;
    }
    size_t ret = std::numeric_limits<size_t>::max();
    for (size_t k = i; k < j; k++) {
        size_t q = RecursiveMatrixChain(p, i, k) + RecursiveMatrixChain(p, k + 1, j)
                + p[i] * p[k + 1] * p[j + 1];
        if (q < ret) {
            ret = q;
        }
    }
    return ret;
}

size_t LookupChain(std::vector<std::vector<size_t>>& m, const std::vector<size_t>& p, size_t i, size_t j) {
    if (m[i][j] < std::numeric_limits<size_t>::max()) {
        return m[i][j];
    }
    size_t& ret = m[i][j];
    if (i == j) {
        ret = 0;
    } else {
        for (size_t k = i; k < j - 1; k++) {
            size_t q = LookupChain(m, p, i, k) + LookupChain(m, p, k + 1, j)
                    + p[i] * p[k + 1] * p[j + 1];
            if (q < ret) {
                ret = q;
            }
        }
    }
    return ret;
}

size_t MemoizedMatrixChain(const std::vector<size_t>& p) {
    assert(p.size() > 1);
    size_t n = p.size() - 1;
    std::vector<std::vector<size_t>> m (n, std::vector<size_t>(n));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i; j < n; j++) {
            m[i][j] = std::numeric_limits<size_t>::max();
        }
    }
    return LookupChain(m, p, 0, n - 1);
}

int main() {
    std::vector<size_t> p {30, 35, 15, 5, 10, 20, 25};


}