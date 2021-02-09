#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <typename T>
std::vector<T> Select(std::vector<T>& A, size_t i) {
    if (i >= A.size() / 2) {
        sr::nth_element(A, A.begin() + i);
        std::vector<T> B (A.begin(), A.begin() + i);
        return B;
    } else {
        size_t m = A.size() / 2;
        for (size_t j = 0; j < m; j++) {
            if (A[j] < A[j + m]) {
                std::swap(A[j], A[j + m]);
            }
        }
        sr::nth_element(A | srv::drop(m), A.begin() + m + i);
        sr::nth_element(A, A.begin() + i);
        std::vector<T> B;
        for (size_t j = 0; j < i; j++) {
            B.push_back(A[j]);
            B.push_back(A[m + j]);
        }
        sr::nth_element(B, B.begin() + i);
        std::vector<T> C (std::make_move_iterator(B.begin()), std::make_move_iterator(B.begin() + i));
        return C;
    }
}

int main() {
    std::vector<int> v {7, 9, 1, 3, 6, 5, 8, 4, 10, 2};
    auto w = Select(v, 3);
    for (auto n : w) {
        std::cout << n << ' ';
    }

}