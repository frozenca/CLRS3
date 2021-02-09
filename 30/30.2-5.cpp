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
namespace sn = std::numbers;

std::vector<std::complex<double>> RecursiveFFT(std::vector<double>& A) {
    using namespace std::complex_literals;
    std::size_t n = A.size();
    if (n == 1) {
        return std::vector<std::complex<double>> {A[0]};
    }
    auto w_n = std::exp(2.0i * sn::pi / static_cast<double>(n));
    auto w_3 = std::exp(2.0i * sn::pi / 3.0);
    auto w_32 = std::exp(2.0i * sn::pi * 2.0 / 3.0);
    std::complex<double> w = 1;
    std::vector<double> A0 (n / 3);
    std::vector<double> A1 (n / 3);
    std::vector<double> A2 (n / 3);
    for (std::size_t i = 0; i < n; i += 3) {
        A0[i / 3] = A[i];
        A1[i / 3] = A[i + 1];
        A2[i / 3] = A[i + 2];
    }
    auto y0 = RecursiveFFT(A0);
    auto y1 = RecursiveFFT(A1);
    auto y2 = RecursiveFFT(A2);
    std::vector<std::complex<double>> y (n);
    for (std::size_t k = 0; k < n / 3; k++) {
        auto w2 = w * w;
        y[k] = y0[k] + w * y1[k] + w2 * y2[k];
        y[k + n / 3] = y0[k] + w_3 * w * y1[k] + w_32 * w2 * y2[k];
        y[k + 2 * n / 3] = y0[k] + w_32 * w * y1[k] + w_3 * w2 * y2[k];
        w *= w_n;
    }
    return y;
}

std::vector<std::complex<double>> RecursiveFFT2(std::vector<double>& A) {
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
    auto y0 = RecursiveFFT2(A0);
    auto y1 = RecursiveFFT2(A1);
    std::vector<std::complex<double>> y (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        y[k] = y0[k] + w * y1[k];
        y[k + n / 2] = y0[k] - w * y1[k];
        w *= w_n;
    }
    return y;
}


int main() {
    std::vector<double> A {1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto A_ = RecursiveFFT(A);
    for (auto a : A_) {
        std::cout << a << ' ';
    }
    std::cout << '\n';
}