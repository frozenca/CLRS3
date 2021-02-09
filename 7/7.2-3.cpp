#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
size_t partition(std::vector<T>& A, size_t p, size_t r) {
    T x = A[r];
    size_t i = p;
    size_t equal_count = 0;
    for (size_t j = p; j < r; j++) {
        if (A[j] >= x) {
            std::swap(A[i], A[j]);
            i++;
        }
    }
    std::swap(A[i], A[r]);
    return i;
}

template <typename T>
void quickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        size_t q = partition(A, p, r);
        quickSort(A, p, q - 1);
        quickSort(A, q + 1, r);
    }
}

int main() {
    constexpr size_t SIZE = 50'000;
    std::vector<int> v (SIZE);
    std::iota(v.begin(), v.end(), 0);
    sr::reverse(v);
    quickSort(v, 0, v.size() - 1); // very slow
}