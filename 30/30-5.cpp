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

using Comp = std::complex<double>;
using Vec = std::vector<std::complex<double>>;

Vec PolyAdd(const Vec& A, const Vec& B) {
    std::size_t n = std::max(A.size(), B.size());
    Vec C (n);
    Vec A_ (n), B_ (n);
    sr::copy(A, A_.begin());
    sr::copy(B, B_.begin());
    sr::transform(A_, B_, C.begin(), std::plus{});
    while (!C.empty() && std::abs(C.back()) < 1e-9) {
        C.pop_back();
    }
    return C;
}

Vec PolySub(const Vec& A, const Vec& B) {
    std::size_t n = std::max(A.size(), B.size());
    Vec C (n);
    Vec A_ (n), B_ (n);
    sr::copy(A, A_.begin());
    sr::copy(B, B_.begin());
    sr::transform(A_, B_, C.begin(), std::minus{});
    while (!C.empty() && std::abs(C.back()) < 1e-9) {
        C.pop_back();
    }
    return C;
}

Vec PolyScale(const Vec& A, std::size_t n, bool div = false) {
    if (!div) {
        Vec A_ (A.size() + n);
        sr::copy(A, A_.begin() + n);
        return A_;
    } else {
        if (A.size() > n) {
            Vec A_ (A.begin() + n, A.end());
            return A_;
        } else {
            return Vec{0};
        }
    }
}

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

Vec PolyReciprocal(const Vec& p) {
    std::size_t k = p.size();
    if (k == 1) {
        return Vec{1.0 / p[0]};
    }
    assert(!p.empty() && (k & (k - 1)) == 0);
    Vec p_right_half (p.begin() + k / 2, p.end());
    auto q = PolyReciprocal(p_right_half);
    auto two_q = q;
    sr::for_each(two_q, [](auto& c){c *= 2.0;});

    Vec r_first = PolyScale(two_q, k * 3 / 2 - 2);
    Vec r_second = PolynomialProduct(PolynomialProduct(q, q), p);
    return PolyScale(PolySub(r_first, r_second), k - 2, true);
}

std::size_t two_ceil(std::size_t sz) {
    return 1u << static_cast<std::size_t>(std::ceil(std::log2(sz)));
}

std::pair<Vec, Vec> PolyDivision(const Vec& u, const Vec& v) {
    assert(!v.empty() && !u.empty());
    if (v.size() > u.size()) {
        return {Vec{0}, u};
    } else if (v.size() == u.size()) {
        Comp coeff = u.back() / v.back();
        if (v.size() == 1) {
            return {Vec{coeff}, Vec{0}};
        }
        auto v_mult = v;
        sr::for_each(v_mult, [&coeff](auto& c){c *= coeff;});
        auto p = Vec{coeff};
        auto q = PolySub(u, v_mult);
        q.resize(v.size() - 1);
        return {Vec{coeff}, q};
    }
    std::size_t n = two_ceil(v.size()) - 1;
    std::size_t nd = (n + 1) - v.size();
    std::size_t m = u.size() + nd - 1;
    Vec v_scaled = PolyScale(v, nd);
    Vec u_scaled = PolyScale(u, nd);
    auto v_inv = PolyReciprocal(v_scaled);
    Vec q = PolyScale(PolynomialProduct(u_scaled, v_inv), 2 * n, true);

    if (m > 2 * n) {
        auto t = PolySub(PolyScale(Vec{1.0}, 2 * n), PolynomialProduct(v_inv, v_scaled));
        auto [q2, r2] = PolyDivision(PolyScale(PolynomialProduct(u_scaled, t), 2 * n, true), v_scaled);
        q = PolyAdd(q, q2);
    }

    auto r = PolySub(u, PolynomialProduct(v, q));
    return {q, r};
}

Vec PolyMultipleEval(Vec& A, const Vec& x) {
    std::size_t n = x.size();
    assert(A.size() - 1 <= n && !x.empty() & !A.empty());
    std::size_t sz = two_ceil(n);
    std::vector<Vec> P (2 * sz - 1);
    for (std::size_t k = sz - 1; k < sz - 1 + n; k++) {
        P[k] = Vec{-x[k - (sz - 1)], 1.0};
    }
    for (std::size_t k = sz - 1 + n; k < 2 * sz - 1; k++) {
        P[k] = Vec{1.0};
    }
    auto lg_sz = static_cast<std::size_t>(std::log2(sz));
    std::size_t l = sz - 1;
    std::size_t r = 2 * sz - 1;
    for (std::size_t k = 0; k < lg_sz; k++) {
        for (std::size_t i = l; i < r; i += 2) {
            P[i / 2] = PolynomialProduct(P[i], P[i + 1]);
        }
        l /= 2;
        r /= 2;
    }
    std::vector<Vec> Q (2 * sz - 1);
    Q[0] = A;
    l = 1;
    r = 3;
    for (std::size_t k = 0; k < lg_sz; k++) {
        for (std::size_t i = l; i < r; i += 2) {
            std::tie(std::ignore, Q[i]) = PolyDivision(Q[i / 2], P[i]);
            std::tie(std::ignore, Q[i + 1]) = PolyDivision(Q[i / 2], P[i + 1]);
        }
        l = 2 * l + 1;
        r = 2 * r + 1;
    }
    Vec vals(n);
    for (std::size_t k = sz - 1; k < sz + n - 1; k++) {
        vals[k - (sz - 1)] = Q[k][0];
    }
    return vals;
}

int main() {
    Vec A {1.0, 1.0, 1.0, 1.0, 1.0};
    Vec x {1.0, 2.0, 3.0, 4.0, 5.0};
    auto vals = PolyMultipleEval(A, x);
    for (auto& val : vals) {
        std::cout << val << ' ';
    }
    std::cout << '\n';

}