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
#include <thread>

namespace se = std::execution;
namespace sr = std::ranges;
namespace sn = std::numbers;

uint32_t rev(uint32_t n, uint32_t pow) {
    uint32_t ret = 0;
    while (n) {
        ret += (n & 1) << pow;
        n >>= 1;
        pow--;
    }
    return ret;
}

std::vector<std::complex<double>> bitReverseCopy(const std::vector<double>& a) {
    std::size_t n = a.size();
    std::vector<std::complex<double>> A(n);
    auto pow = std::floor(std::log2(n)) - 1;
    for (std::size_t k = 0; k < n; k++) {
        A[rev(k, pow)] = a[k];
    }
    return A;
}

std::vector<std::complex<double>> bitReverseCopy(const std::vector<std::complex<double>>& a) {
    std::size_t n = a.size();
    std::vector<std::complex<double>> A(n);
    auto pow = std::floor(std::log2(n)) - 1;
    for (std::size_t k = 0; k < n; k++) {
        A[rev(k, pow)] = a[k];
    }
    return A;
}

std::vector<std::complex<double>> IterativeFFT(const std::vector<double>& a) {
    using namespace std::complex_literals;
    std::size_t n = a.size();
    auto A = bitReverseCopy(a);
    auto lgn = std::floor(std::log2(n));
    for (std::size_t s = 1; s <= lgn; s++) {
        std::size_t m = (1u << s);
        auto w_m = std::exp(2.0i * sn::pi / static_cast<double>(m));
        for (std::size_t k = 0; k < n; k += m) {
            std::complex<double> w = 1;
            for (std::size_t j = 0; j < m / 2; j++) {
                auto t = w * A[k + j + m / 2];
                auto u = A[k + j];
                A[k + j] = u + t;
                A[k + j + m / 2] = u - t;
                w *= w_m;
            }
        }
    }
    return A;
}

std::vector<std::complex<double>> IterativeFFT2(const std::vector<double>& a) {
    using namespace std::complex_literals;
    std::size_t n = a.size();
    std::size_t lgn = std::floor(std::log2(n));
    std::vector<std::complex<double>> A;
    A.reserve(a.size());
    for (auto a_elt : a) {
        A.emplace_back(a_elt);
    }
    std::size_t pow = std::floor(std::log2(n)) - 1;
    for (std::size_t s = lgn; s >= 1; s--) {
        std::size_t m = (1u << s);
        auto w_inv = std::exp(2.0i * sn::pi / static_cast<double>(2 * n / m));
        for (std::size_t k = 0; k < n; k += m) {
            std::complex<double> w = std::pow(w_inv, rev(k, pow));
            for (std::size_t j = 0; j < m / 2; j++) {
                auto t = w * A[k + j + m / 2];
                auto u = A[k + j];
                A[k + j] = u + t;
                A[k + j + m / 2] = u - t;
            }
        }
    }
    return bitReverseCopy(A);
}

int main() {
    std::vector<double> v {0, 2, 3, -1, 4, 5, 7, 9};

    for (const auto& n : v) {
        std::cout << n << ' ';
    }
    std::cout << '\n';

    auto W = IterativeFFT(v);
    for (const auto& w : W) {
        std::cout << w << ' ';
    }
    std::cout << '\n';

    auto W2 = IterativeFFT2(v);
    for (const auto& w : W2) {
        std::cout << w << ' ';
    }
    std::cout << '\n';


}