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

std::vector<std::complex<double>> RecursiveFFT(const std::vector<double>& A) {
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

std::vector<std::complex<double>> InverseDFT(const std::vector<std::complex<double>>& y) {
    using namespace std::complex_literals;
    std::size_t n = y.size();
    if (n == 1) {
        return std::vector<std::complex<double>> {y[0]};
    }
    auto w_n = std::exp(2.0i * sn::pi / (1.0 * n));
    std::complex<double> w = 1.0;
    std::vector<std::complex<double>> y0 (n / 2);
    std::vector<std::complex<double>> y1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        y0[i / 2] = y[i];
        y1[i / 2] = y[i + 1];
    }
    auto a0 = InverseDFT(y0);
    auto a1 = InverseDFT(y1);
    std::vector<std::complex<double>> A (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        A[k] = a0[k] + w * a1[k];
        A[k + n / 2] = a0[k] - w * a1[k];
        w *= (1.0 / w_n);
    }
    for (std::size_t k = 0; k < n; k++) {
        A[k] /= 2.0;
    }
    return A;
}

int main() {
    std::vector<double> A {-10, 1, -1, 7, 0, 0, 0, 0};
    std::vector<double> B {3, -6, 0, 8, 0, 0, 0, 0};

    auto A_ = RecursiveFFT(A);
    auto B_ = RecursiveFFT(B);
    std::vector<std::complex<double>> AB_ (A.size());
    sr::transform(A_, B_, AB_.begin(), std::multiplies{});

    auto AB = InverseDFT(AB_);

    for (auto ab : AB) {
        std::cout << ab << ' ';
    }
    std::cout << '\n';



}