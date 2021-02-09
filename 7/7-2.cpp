#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
std::pair<size_t, size_t> partition(std::vector<T>& A, size_t p, size_t r) {
    T x = A[r];
    size_t l = p, m = p, h = r;
    while (m <= h) {
        if (A[m] < x) {
            std::swap(A[l], A[m]);
            l++;
            m++;
        } else if (A[m] > x) {
            std::swap(A[m], A[h]);
            h--;
        } else {
            m++;
        }
    }
    return {l, m - 1};
}

template <typename T>
void quickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        auto [l, m] = partition(A, p, r);
        quickSort(A, p, l - 1);
        quickSort(A, m + 1, r);
    }
}

int main() {
    std::vector<int> v {3, 2, 6, 1, 5, 4};
    quickSort(v, 0, v.size() - 1);
    assert(sr::is_sorted(v));
}