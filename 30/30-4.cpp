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

std::size_t two_ceil(std::size_t sz) {
    return 1u << static_cast<std::size_t>(std::ceil(std::log2(sz)));
}

Vec EvaluateDerivatives(const Vec& A, const Comp& x_0) {
    std::size_t n = A.size();
    std::size_t sz = two_ceil(n);
    Vec f(2 * sz);
    Vec g(2 * sz);
    Comp fac = 1.0;
    for (std::size_t j = 0; j < n; j++) {
        f[j] = A[j] * fac;
        fac *= (j + 1.0);
    }
    Comp g_val = 1.0;
    for (std::size_t j = 0; j < n; j++) {
        g[n - 1 - j] = g_val;
        g_val *= (x_0 / (j + 1.0));
    }
    Vec h = PolynomialProduct(f, g);
    Vec c (sz);
    fac = 1.0;
    for (std::size_t r = 0; r < n; r++) {
        c[r] = h[n - 1 + r] / fac;
        fac *= (r + 1.0);
    }
    Vec A_eval = RecursiveFFT(c);
    Vec B = InverseDFT(A_eval);
    Vec derivs (sz);
    fac = 1.0;
    for (std::size_t t = 0; t < n; t++) {
        derivs[t] = B[t] * fac;
        fac *= (t + 1.0);
    }
    return derivs;
}

int main() {
    Vec A {1, 1, 1, 1, 1, 1};
    Vec A_derivs = EvaluateDerivatives(A, 1.0);
    for (auto& A_t_x0 : A_derivs) {
        std::cout << A_t_x0 << ' ';
    }
    std::cout << '\n';
}