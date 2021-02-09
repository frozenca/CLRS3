#include <cassert>
#include <random>
#include <algorithm>
#include <functional>
#include <iostream>
#include <ranges>
#include <utility>
#include <vector>
#include <thread>

std::mt19937 gen(std::random_device{}());

namespace sr = std::ranges;

template <typename T>
void PPartition(std::vector<T>& A, size_t p, size_t r, std::size_t& piv_index, std::size_t& res) {
    if (p > r) {
        res = 0;
        return;
    } else if (p == r) {
        res = A[p] < A[piv_index];
        return;
    }
    std::size_t m = (p + r) / 2;
    std::size_t i = 0, j = 0;
    {
        std::jthread th(&PPartition<T>, std::ref(A), p, m, std::ref(piv_index), std::ref(i));
        PPartition(A, m + 1, r, piv_index, j);
    }
    for (std::size_t k = 0; k < std::min(j, m - p - i + 1); k++) {
        std::swap(A[p + i + k], A[m + j - k]);
    }
    if (piv_index >= p && piv_index <= r) {
        std::swap(A[p + i + j], A[piv_index]);
        piv_index = p + i + j;
    }
    res = i + j;
}

template <typename T>
void PRandomizedPartition(std::vector<T>& A, std::size_t p, std::size_t r, std::size_t& pivot_index) {
    std::uniform_int_distribution<> dst (p, r);
    std::size_t i = dst(gen);
    std::swap(A[i], A[r]);
    pivot_index = r;
    std::size_t res = 0;
    PPartition(A, p, r, pivot_index, res);
}

template <typename T>
T PRandomizedSelect(std::vector<T>& A, std::size_t p, std::size_t r, std::size_t i) {
    if (p == r) {
        return A[p];
    }
    std::size_t q = 0;
    PRandomizedPartition(A, p, r, q);
    std::size_t k = q - p + 1;
    if (i == k) {
        return A[q];
    } else if (i < k) {
        return PRandomizedSelect(A, p, q - 1, i);
    } else {
        return PRandomizedSelect(A, q + 1, r, i - k);
    }
}

int main() {
    constexpr std::size_t N = 20;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    sr::shuffle(v, gen);
    std::cout << PRandomizedSelect(v, 0, v.size() - 1, v.size() / 2);

}