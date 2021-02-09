#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <iostream>
#include <vector>
#include <ranges>

std::mt19937 gen(std::random_device{}());

namespace sr = std::ranges;

template <typename T>
void makeMedian(std::vector<T>& A, std::size_t p, std::size_t r) {
    std::nth_element(A.begin() + p, A.begin() + (p + r) / 2, A.begin() + r);
}

template <typename T>
std::size_t Partition(std::vector<T>& A, std::size_t p, std::size_t r, const T& pivot) {
    std::size_t i = p;
    std::size_t j = p;
    std::size_t k = r;
    while (j <= k) {
        if (A[j] < pivot) {
            std::swap(A[i++], A[j++]);
        } else if (A[j] > pivot) {
            std::swap(A[j], A[k--]);
        } else {
            j++;
        }
    }
    return i;
}

template <typename T>
T Select(std::vector<T>& A, std::size_t p, std::size_t r, std::size_t i) {
    if (p == r) {
        return A[p];
    }
    std::size_t size = r - p + 1;
    if (size <= 5) {
        std::nth_element(A.begin() + p, A.begin() + p + i, A.begin() + r + 1);
        return A[p + i];
    }
    std::size_t groups = std::ceil(size / 5);
    std::size_t full_groups = std::floor(size / 5);
    std::vector<T> B (groups);
    for (std::size_t g = 0; g < full_groups; g++) {
        makeMedian(A, p + 5 * g, p + 5 * g + 5);
        B[g] = A[p + 5 * g + 2];
    }
    if (groups > full_groups) {
        makeMedian(A, p + 5 * full_groups, r + 1);
        B.back() = A[p + 5 * full_groups + (size % 5) / 2];
    }
    auto x = Select(B, 0, groups - 1, groups / 2);
    std::size_t q = Partition(A, p, r, x);
    auto k = q - p;
    if (i == k) {
        return x;
    } else if (i < k) {
        return Select(A, p, q - 1, i);
    } else {
        return Select(A, q + 1, r, i - k - 1);
    }
}

int main() {
    constexpr std::size_t N = 2376;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    sr::shuffle(v, gen);

    std::cout << Select(v, 0, N - 1, N / 2);

}