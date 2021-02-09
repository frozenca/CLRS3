#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <typename T>
size_t partition(std::vector<T>& A, size_t p, size_t r) {
    sr::nth_element(A | srv::drop(p) | srv::take(r), A.begin() + (p + r) / 2);
    T x = A[(p + r) / 2];
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
    std::vector<int> v {4, 2, 1, 5, 3, 6};
    quickSort(v, 0, v.size() - 1);
    assert(sr::is_sorted(v));

}