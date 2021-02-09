#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
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

int main() {
    constexpr double alpha = 0.3;
    constexpr size_t trials = 1'000;
    constexpr size_t N = 1'000;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    std::mt19937 gen(std::random_device{}());

    size_t balanced_count = 0;
    for (size_t i = 0; i < trials; i++) {
        auto u = v;
        sr::shuffle(u, gen);
        size_t q = partition(u, 0, u.size() - 1);

        if (q < (1.0 - alpha) * N && q > (alpha) * N) {
            balanced_count++;
        }
    }

    std::cout << "Probability that the partition is balanced than (1-a)/a : "
              << static_cast<double>(balanced_count) / static_cast<double>(trials) << '\n';
    std::cout << "1 - 2a : " << 1.0 - 2 * alpha << '\n';

}