#include <algorithm>
#include <cassert>
#include <execution>
#include <iostream>
#include <vector>
#include <utility>
#include <numeric>
#include <random>
#include <ranges>
#include <thread>

namespace se = std::execution;
namespace sr = std::ranges;

template <typename T>
size_t Partition(std::vector<T>& A, size_t p, size_t r) {
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

std::vector<std::size_t> global_index;

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
    std::for_each(se::unseq, global_index.begin(), global_index.begin() + std::min(j, m - p - i + 1), [&A, &i, &j, &p, &m](auto k) {
        std::swap(A[p + i + k], A[m + j - k]);
    });
    if (piv_index >= p && piv_index <= r) {
        std::swap(A[p + i + j], A[piv_index]);
        piv_index = p + i + j;
    }
    res = i + j;
}

int main() {
    constexpr std::size_t N = 20;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    global_index.resize(N);
    std::iota(global_index.begin(), global_index.end(), 0);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);
    std::cout << v.back() << '\n';

    auto v2 = v;
    Partition(v, 0, N - 1);
    for (auto n : v) {
        std::cout << n << ' ';
    }
    std::cout << '\n';

    std::size_t piv_index = N - 1;
    std::size_t res = 0;
    PPartition(v2, 0, N - 1, piv_index, res);
    for (auto n : v2) {
        std::cout << n << ' ';
    }
    std::cout << '\n';

}