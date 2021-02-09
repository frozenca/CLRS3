#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <numeric>
#include <vector>

template <typename T>
std::optional<T> findMedian(const std::vector<T>& A, const std::vector<T>& B, size_t low, size_t high) {
    if (low > high) {
        return {};
    } else {
        size_t k = (low + high) / 2;
        if (k == A.size() - 1 && A[A.size() - 1] <= B[0]) {
            return A[A.size() - 1];
        } else if (k < A.size() - 1 && B[B.size() - 1 - k] <= A[k] && A[k] <= B[B.size() - k]) {
            return A[k];
        } else if (A[k] > B[B.size() - k]) {
            return findMedian(A, B, low, k - 1);
        } else {
            return findMedian(A, B, k + 1, high);
        }
    }
}


template <typename T>
T twoArrayMedian(const std::vector<T>& A, const std::vector<T>& B) {
    T median = findMedian(A, B, 0, A.size() - 1).value();
    if (!median) {
        median = findMedian(B, A, 0, B.size() - 1).value();
    }
    return median;
}

int main() {
    std::vector<int> A (10);
    std::iota(A.begin(), A.end(), 4);
    std::vector<int> B (10);
    std::iota(B.begin(), B.end(), 8);
    std::cout << twoArrayMedian(A, B);

}