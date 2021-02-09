#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <typename T>
size_t partition(std::vector<T>& A, size_t p, size_t r, const T& x) {
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
T Select(std::vector<T>& A, size_t p, size_t r, size_t i) {
    if (p == r) {
        return A[p];
    }
    sr::nth_element(A | srv::drop(p) | srv::take(r + 1), A.begin() + (p + r + 1) / 2);
    T x = A[(p + r + 1) / 2];
    size_t q = partition(A, p, r, x);
    size_t k = q - p + 1;
    if (i == k) {
        return A[q];
    } else if (i < k) {
        return Select(A, p, q - 1, i);
    } else {
        return Select(A, q + 1, r, i - k);
    }
}

int main() {
    std::vector<int> v {4, 2, 1, 5, 3, 6};
    std::cout << Select(v, 0, v.size() - 1, 4);


}