#include <algorithm>
#include <cassert>
#include <random>
#include <functional>
#include <vector>
#include <ranges>
#include <thread>

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
void PRandomizedQuickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        size_t q = randomizedPartition(A, p, r);
        std::jthread t (&PRandomizedQuickSort<T>, std::ref(A), p, q - 1);
        PRandomizedQuickSort(A, q + 1, r);
    }
}

int main() {
    std::vector<int> v {3, 2, 6, 1, 5, 4};
    PRandomizedQuickSort(v, 0, v.size() - 1);
    assert(sr::is_sorted(v));
}