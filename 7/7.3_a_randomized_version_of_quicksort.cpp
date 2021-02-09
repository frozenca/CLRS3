#include <algorithm>
#include <cassert>
#include <random>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

template <typename T>
size_t partition(std::vector<T>& A, size_t p, size_t r) {
    T x = A[r];
    size_t i = p;
    for (size_t j = p; j < r; j++) {
        if (A[j] <= x) {
            std::swap(A[i], A[j]);
            i++;
        }
    }
    std::swap(A[i], A[r]);
    return i;
}

template <typename T>
size_t randomizedPartition(std::vector<T>& A, size_t p, size_t r) {
    std::uniform_int_distribution<> dst (p, r);
    size_t i = dst(gen);
    std::swap(A[i], A[r]);
    return partition(A, p, r);
}

template <typename T>
void randomizedQuickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        size_t q = randomizedPartition(A, p, r);
        randomizedQuickSort(A, p, q - 1);
        randomizedQuickSort(A, q + 1, r);
    }
}

int main() {
    std::vector<int> v {3, 2, 6, 1, 5, 4};
    randomizedQuickSort(v, 0, v.size() - 1);
    assert(sr::is_sorted(v));
}