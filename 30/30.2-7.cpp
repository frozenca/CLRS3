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

std::vector<double> PolynomialProduct(const std::vector<double>& A, const std::vector<double>& B) {
    std::size_t big_size = std::max(A.size(), B.size());
    std::size_t poly_size = (1u << (static_cast<std::size_t>(std::ceil(std::log2(big_size))) + 1));
    std::vector<double> A_poly = A;
    std::vector<double> B_poly = B;
    for (std::size_t k = 0; k < poly_size - A.size(); k++) {
        A_poly.push_back(0);
    }
    for (std::size_t k = 0; k < poly_size - B.size(); k++) {
        B_poly.push_back(0);
    }
    auto A_ = RecursiveFFT(A_poly);
    auto B_ = RecursiveFFT(B_poly);
    std::vector<std::complex<double>> AB_ (poly_size);
    sr::transform(A_, B_, AB_.begin(), std::multiplies{});

    auto AB = InverseDFT(AB_);
    std::vector<double> AB_real (AB.size());
    sr::transform(AB, AB_real.begin(), [](auto c) -> double { return std::real(c);});
    while (!AB_real.empty() && std::abs(AB_real.back()) < 1e-8) {
        AB_real.pop_back();
    }
    return AB_real;
}

std::vector<double> PolyProductRecursive(const std::vector<std::vector<double>>& polys, std::size_t p, std::size_t q) {
    if (p == q) {
        return polys[p];
    }
    std::size_t r = (p + q) / 2;
    auto poly1 = PolyProductRecursive(polys, p, r);
    auto poly2 = PolyProductRecursive(polys, r + 1, q);
    return PolynomialProduct(poly1, poly2);

}

std::vector<double> FindPolynomialWithRoots(const std::vector<double>& roots) {
    assert(!roots.empty());
    std::size_t two_ceil = (1u << static_cast<std::size_t>(std::ceil(std::log2(roots.size()))));
    std::vector<std::vector<double>> polys;
    polys.reserve(two_ceil);
    for (auto root : roots) {
        polys.push_back(std::vector<double>{-root, 1});
    }
    for (std::size_t i = 0; i < two_ceil - roots.size(); i++) {
        polys.push_back(std::vector<double>{1});
    }
    return PolyProductRecursive(polys, 0, polys.size() - 1);
}

int main() {
    std::vector<double> roots {1.0, 2.0, 3.0, 4.0, 5.0};
    auto poly = FindPolynomialWithRoots(roots);

    for (auto coeff : poly) {
        std::cout << coeff << ' ';
    }
    std::cout << '\n';
}