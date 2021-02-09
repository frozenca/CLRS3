#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <execution>
#include <functional>
#include <iostream>
#include <vector>
#include <numbers>
#include <numeric>
#include <random>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;
namespace sn = std::numbers;

using IntVec = std::vector<std::size_t>;

constexpr std::size_t p = 17;
constexpr std::size_t g_k = 9;
constexpr std::size_t g_inv_k = 2;

IntVec IntegerFFT(const IntVec& A) {
    std::size_t n = A.size();
    if (n == 1) {
        return IntVec {A[0]};
    }
    auto w_n = g_k;
    std::size_t w = 1;
    IntVec A0 (n / 2);
    IntVec A1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        A0[i / 2] = A[i];
        A1[i / 2] = A[i + 1];
    }
    auto y0 = IntegerFFT(A0);
    auto y1 = IntegerFFT(A1);
    IntVec y (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        y[k] = (y0[k] + w * y1[k]) % p;
        y[k + n / 2] = (y0[k] + w * (p - 1) * y1[k]) % p;
        w = (w * w_n) % p;
    }
    return y;
}

IntVec IntegerInverseFFT(const IntVec& y) {
    std::size_t n = y.size();
    if (n == 1) {
        return IntVec {y[0]};
    }
    auto w_inv = g_inv_k;
    std::size_t w = 1;
    IntVec y0 (n / 2);
    IntVec y1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        y0[i / 2] = y[i];
        y1[i / 2] = y[i + 1];
    }
    auto a0 = IntegerInverseFFT(y0);
    auto a1 = IntegerInverseFFT(y1);
    IntVec A (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        A[k] = (a0[k] + w * a1[k]) % p;
        A[k + n / 2] = (a0[k] + w * (p - 1) * a1[k]) % p;
        w = (w * w_inv) % p;
    }
    for (std::size_t k = 0; k < n; k++) {
        A[k] = (A[k] * (p + 1) / 2) % p;
    }
    return A;
}

int main() {
    IntVec A {0, 5, 3, 7, 7, 2, 1, 6};
    auto Y = IntegerFFT(A);
    for (auto y : Y) {
        std::cout << y << ' ';
    }
    std::cout << '\n';
}
