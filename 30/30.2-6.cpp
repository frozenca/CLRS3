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

std::vector<std::size_t> IntegerFFT(const std::vector<std::size_t>& A, std::size_t t, std::size_t N, std::size_t m) {
    std::size_t n = A.size();
    if (n == 1) {
        return std::vector<std::size_t> {A[0]};
    }
    auto w_n = (1u << (t * N / n));
    std::size_t w = 1;
    std::vector<std::size_t> A0 (n / 2);
    std::vector<std::size_t> A1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        A0[i / 2] = A[i];
        A1[i / 2] = A[i + 1];
    }
    auto y0 = IntegerFFT(A0, t, N, m);
    auto y1 = IntegerFFT(A1, t, N, m);
    std::vector<std::size_t> y (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        y[k] = (y0[k] + w * y1[k]) % m;
        y[k + n / 2] = (y0[k] + w * (m - 1) * y1[k]) % m;
        w = (w * w_n) % m;
    }
    return y;
}

std::vector<std::size_t> IntegerDFT(const std::vector<std::size_t>& y, std::size_t t, std::size_t N, std::size_t m) {
    std::size_t n = y.size();
    if (n == 1) {
        return std::vector<std::size_t> {y[0]};
    }
    auto w_inv = (1u << (t * (n - 1) * N / n)) % m;
    std::size_t w = 1;
    std::vector<std::size_t> y0 (n / 2);
    std::vector<std::size_t> y1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        y0[i / 2] = y[i];
        y1[i / 2] = y[i + 1];
    }
    auto a0 = IntegerDFT(y0, t, N, m);
    auto a1 = IntegerDFT(y1, t, N, m);
    std::vector<std::size_t> A (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        A[k] = (a0[k] + w * a1[k]) % m;
        A[k + n / 2] = (a0[k] + w * (m - 1) * a1[k]) % m;
        w = (w * w_inv) % m;
    }
    for (std::size_t k = 0; k < n; k++) {
        A[k] = (A[k] * (m + 1) / 2) % m;
    }
    return A;
}

int main() {
    std::vector<double> A {10, 1, 1, 7, 0, 0, 0, 0};
    std::vector<double> B {3, 6, 0, 8, 0, 0, 0, 0};

    auto A_ = RecursiveFFT(A);
    auto B_ = RecursiveFFT(B);
    std::vector<std::complex<double>> AB_ (A.size());
    sr::transform(A_, B_, AB_.begin(), std::multiplies{});

    auto AB = InverseDFT(AB_);

    for (auto ab : AB) {
        std::cout << ab << ' ';
    }
    std::cout << '\n';

    constexpr std::size_t t = 2;

    std::vector<std::size_t> A2 {10, 1, 1, 7, 0, 0, 0, 0};
    std::vector<std::size_t> B2 {3, 6, 0, 8, 0, 0, 0, 0};

    std::size_t N = A2.size();
    std::size_t m = (1u << (t * N / 2)) + 1;

    auto A2_ = IntegerFFT(A2, t, N, m);
    auto B2_ = IntegerFFT(B2, t, N, m);
    std::vector<std::size_t> AB2_ (A2.size());
    sr::transform(A2_, B2_, AB2_.begin(), std::multiplies{});

    auto AB2 = IntegerDFT(AB2_, t, N, m);

    for (auto ab : AB2) {
        std::cout << ab << ' ';
    }
    std::cout << '\n';


}