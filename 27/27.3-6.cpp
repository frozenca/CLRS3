#include <algorithm>
#include <cassert>
#include <cmath>
#include <execution>
#include <random>
#include <iostream>
#include <vector>
#include <ranges>
#include <thread>

std::mt19937 gen(std::random_device{}());

namespace sr = std::ranges;
namespace se = std::execution;

template <typename T>
void makeMedian(std::vector<T>& A, std::size_t p, std::size_t r) {
    std::nth_element(se::unseq, A.begin() + p, A.begin() + (p + r) / 2, A.begin() + r);
}

std::vector<std::size_t> global_index;

template <typename T>
void PPartition(std::vector<T>& A, size_t p, size_t r, const T& pivot, std::size_t& less, std::size_t& same) {
    if (p == r) {
        less = A[p] < pivot;
        same = A[p] == pivot;
        return;
    }
    std::size_t m = (p + r) / 2;
    std::size_t less1 = 0, less2 = 0, same1 = 0, same2 = 0;
    {
        std::jthread t1 (&PPartition<T>, std::ref(A), p, m, std::cref(pivot), std::ref(less1), std::ref(same1));
        PPartition(A, m + 1, r, pivot, less2, same2);
    }

    if (less2 > m - p + 1 - less1) {
        std::for_each(se::unseq, global_index.begin(), global_index.begin() + m - p - less1 + 1,
                      [&A, &less1, &less2, &p, &m](auto k) {
                          std::swap(A[p + less1 + k], A[m + less2 - k]);
                      });
        std::for_each(se::unseq, global_index.begin(),
                      global_index.begin() + std::min(same1 + same2, (m + same2 - less1 - p) / 2),
                      [&A, &less1, &less2, &same2, &p, &m](auto k) {
                          std::swap(A[p + less1 + less2 + k], A[m + less2 + same2 - k]);
                      });
    } else {
        std::for_each(se::unseq, global_index.begin(), global_index.begin() + less2,
                      [&A, &less1, &less2, &p, &m](auto k) {
                          std::swap(A[p + less1 + k], A[m + less2 - k]);
                      });
        if (less2 > 0) {
            std::for_each(se::unseq, global_index.begin(),
                          global_index.begin() + same1 + same2,
                          [&A, &less1, &less2, &same2, &p, &m](auto k) {
                              std::swap(A[p + less1 + less2 + k], A[m + less2 + same2 - k]);
                          });
        } else {
            std::for_each(se::unseq, global_index.begin(),
                          global_index.begin() + same2,
                          [&A, &less1, &same1, &same2, &p, &m](auto k) {
                              std::swap(A[p + less1 + same1 + k], A[m + same2 - k]);
                          });
        }
    }
    less = less1 + less2;
    same = same1 + same2;
}

template <typename T>
T PSelect(std::vector<T>& A, std::size_t p, std::size_t r, std::size_t i) {
    if (p == r) {
        return A[p];
    }
    std::size_t size = r - p + 1;
    if (size <= 5) {
        std::nth_element(se::unseq, A.begin() + p, A.begin() + p + i, A.begin() + r + 1);
        return A[p + i];
    }
    std::size_t groups = std::ceil(size / 5);
    std::size_t full_groups = std::floor(size / 5);
    std::vector<T> B (groups);
    std::iota(B.begin(), B.end(), 0);
    std::for_each(se::unseq, B.begin(), B.begin() + full_groups, [&](auto& g){
        makeMedian(A, p + 5 * g, p + 5 * g + 5);
        g = A[p + 5 * g + 2];
    });
    if (groups > full_groups) {
        makeMedian(A, p + 5 * full_groups, r + 1);
        B.back() = A[p + 5 * full_groups + (size % 5) / 2];
    }
    auto x = PSelect(B, 0, groups - 1, groups / 2);
    std::size_t q1 = 0, q2 = 0;
    PPartition(A, p, r, x, q1, q2);
    if (i == q1) {
        return x;
    } else if (i < q1) {
        return PSelect(A, p, p + q1 - 1, i);
    } else {
        return PSelect(A, p + q1 + 1, r, i - q1 - 1);
    }
}

int main() {
    constexpr std::size_t N = 2371;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    global_index.resize(N);
    std::iota(global_index.begin(), global_index.end(), 0);
    sr::shuffle(v, gen);

    std::cout << PSelect(v, 0, N - 1, N / 2);

}