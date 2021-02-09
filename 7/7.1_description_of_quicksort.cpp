#include <algorithm>
#include <cassert>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

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
void quickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        size_t q = partition(A, p, r);
        quickSort(A, p, q - 1);
        quickSort(A, q + 1, r);
    }
}

int main() {
    std::vector<int> v {3, 2, 6, 1, 5, 4};
    quickSort(v, 0, v.size() - 1);
    assert(sr::is_sorted(v));
}