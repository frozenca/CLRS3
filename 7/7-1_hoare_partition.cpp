#include <algorithm>
#include <cassert>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
size_t hoarePartition(std::vector<T>& A, size_t p, size_t r) {
    T x = A[p];
    size_t i = p - 1;
    size_t j = r + 1;
    while (true) {
        do {
            j--;
        } while (A[j] > x);
        do {
            i++;
        } while (A[i] < x);
        if (i < j) {
            std::swap(A[i], A[j]);
        } else {
            return j;
        }
    }
}

template <typename T>
void quickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        size_t q = hoarePartition(A, p, r);
        quickSort(A, p, q - 1);
        quickSort(A, q + 1, r);
    }
}

int main() {
    std::vector<int> v {3, 2, 6, 1, 5, 4};
    quickSort(v, 0, v.size() - 1);
    assert(sr::is_sorted(v));
}