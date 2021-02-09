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

std::vector<std::complex<double>> RecursiveFFT(std::vector<double>& A) {
    using namespace std::complex_literals;
    std::size_t n = A.size();
    if (n == 1) {
        return std::vector<std::complex<double>> {A[0]};
    }
    auto w_n = std::exp(2.0i * sn::pi / static_cast<double>(n));
    std::complex<double> w = 1;
    std::vector<double> A0 (n / 2);
    std::vector<double> A1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        A0[i / 2] = A[i];
        A1[i / 2] = A[i + 1];
    }
    auto y0 = RecursiveFFT(A0);
    auto y1 = RecursiveFFT(A1);
    std::vector<std::complex<double>> y (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        y[k] = y0[k] + w * y1[k];
        y[k + n / 2] = y0[k] - w * y1[k];
        w *= w_n;
    }
    return y;
}

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

std::vector<std::complex<double>> IterativeFFT(std::vector<double>& a) {
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

int main() {
    constexpr std::size_t N = 1u << 5u;

    std::vector<double> v (N);
    std::iota(v.begin(), v.end(), 0);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    for (const auto& n : v) {
        std::cout << n << ' ';
    }
    std::cout << '\n';

    auto v2 = v;

    auto W = RecursiveFFT(v);
    for (const auto& w : W) {
        std::cout << w << ' ';
    }
    std::cout << '\n';

    auto W2 = IterativeFFT(v2);
    for (const auto& w : W2) {
        std::cout << w << ' ';
    }
    std::cout << '\n';
    
    assert(W == W2);

}