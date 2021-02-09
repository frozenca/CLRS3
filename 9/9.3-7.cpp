#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
std::vector<T> kClosestToMedian(std::vector<T>& A, size_t k) {
    assert(k <= A.size());
    sr::nth_element(A, A.begin() + A.size() / 2);
    T median = A[A.size() / 2];
    auto B = A;
    sr::transform(B, B.begin(), [median](auto n) {return std::abs(n - median);});
    sr::nth_element(B, B.begin() + k);
    auto diff_threshold = B[k];
    std::vector<T> result;
    for (auto n : A) {
        if (std::abs(n - median) < diff_threshold) {
            result.push_back(n);
        }
    }
    return result;
}

int main() {
    std::vector<int> v {3, 2, 6, 1, 5, 4, 8, 7};
    auto w = kClosestToMedian(v, 3);
    for (auto n : w) {
        std::cout << n << ' ';
    }

}