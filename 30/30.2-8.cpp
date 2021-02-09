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

using Comp = std::complex<double>;
using Vec = std::vector<std::complex<double>>;

Vec RecursiveFFT(const Vec& A) {
    using namespace std::complex_literals;
    std::size_t n = A.size();
    if (n == 1) {
        return Vec {A[0]};
    }
    auto w_n = std::exp(2.0i * sn::pi / static_cast<double>(n));
    Comp w = 1;
    Vec A0 (n / 2);
    Vec A1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        A0[i / 2] = A[i];
        A1[i / 2] = A[i + 1];
    }
    auto y0 = RecursiveFFT(A0);
    auto y1 = RecursiveFFT(A1);
    Vec y (n);
    for (std::size_t k = 0; k < n / 2; k++) {
        y[k] = y0[k] + w * y1[k];
        y[k + n / 2] = y0[k] - w * y1[k];
        w *= w_n;
    }
    return y;
}

Vec InverseDFT(const Vec& y) {
    using namespace std::complex_literals;
    std::size_t n = y.size();
    if (n == 1) {
        return Vec {y[0]};
    }
    auto w_n = std::exp(2.0i * sn::pi / (1.0 * n));
    Comp w = 1.0;
    Vec y0 (n / 2);
    Vec y1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        y0[i / 2] = y[i];
        y1[i / 2] = y[i + 1];
    }
    auto a0 = InverseDFT(y0);
    auto a1 = InverseDFT(y1);
    Vec A (n);
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

Vec PolynomialProduct(const Vec& A, const Vec& B) {
    std::size_t big_size = std::max(A.size(), B.size());
    std::size_t poly_size = (1u << (static_cast<std::size_t>(std::ceil(std::log2(big_size))) + 1));
    Vec A_poly = A;
    Vec B_poly = B;
    for (std::size_t k = 0; k < poly_size - A.size(); k++) {
        A_poly.push_back(0);
    }
    for (std::size_t k = 0; k < poly_size - B.size(); k++) {
        B_poly.push_back(0);
    }
    auto A_ = RecursiveFFT(A_poly);
    auto B_ = RecursiveFFT(B_poly);
    Vec AB_ (poly_size);
    sr::transform(A_, B_, AB_.begin(), std::multiplies{});

    auto AB = InverseDFT(AB_);
    while (!AB.empty() && std::abs(AB.back()) < 1e-8) {
        AB.pop_back();
    }
    return AB;
}

Vec ChirpTransform(const Vec& A, const Comp& z) {
    std::size_t n = (1u << static_cast<std::size_t>(std::ceil(std::log2(A.size()))));
    Vec P (2 * n);
    Vec Q (2 * n);
    for (std::size_t i = 0; i < A.size(); i++) {
        P[i] = A[i] * std::pow(z, static_cast<double>(i * i) / 2.0);
    }
    for (std::size_t i = 0; i < A.size(); i++) {
        Q[i] = std::pow(z, static_cast<double>(i * i) / -2.0);
        Q[A.size() + i] = Q[i];
    }
    Vec R = PolynomialProduct(P, Q);
    Vec Y (A.size());
    for (std::size_t k = 0; k < A.size(); k++) {
        Y[k] = std::pow(z, static_cast<double>(k * k) / 2.0) * R[k + A.size()];
    }
    return Y;
}

int main() {
    using namespace std::complex_literals;
    Vec v {1.0 + 0.0i, 2.0 + 0.0i, 3.0 + 0.0i, 4.0 + 0.0i};
    auto w = std::exp(2.0i * sn::pi / (1.0 * v.size()));
    auto V1 = RecursiveFFT(v);
    auto V2 = ChirpTransform(v, w);
    for (auto v1 : V1) {
        std::cout << v1 << ' ';
    }
    std::cout << '\n';
    for (auto v2 : V2) {
        std::cout << v2 << ' ';
    }
    std::cout << '\n';

}