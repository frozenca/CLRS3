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
    auto w_n = std::exp(2.0i * sn::pi_v<double> / static_cast<double>(n));
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

void PRecursiveFFT(const std::vector<double>& A, std::vector<std::complex<double>>& y) {
    using namespace std::complex_literals;
    std::size_t n = A.size();
    if (n == 1) {
        y[0] = A[0];
        return;
    }
    auto w_n = std::exp(2.0i * sn::pi_v<double> / static_cast<double>(n));
    std::vector<double> A0 (n / 2);
    std::vector<double> A1 (n / 2);
    for (std::size_t i = 0; i < n; i += 2) {
        A0[i / 2] = A[i];
        A1[i / 2] = A[i + 1];
    }
    std::vector<std::complex<double>> y0 (n / 2);
    std::vector<std::complex<double>> y1 (n / 2);
    {
        std::jthread t(PRecursiveFFT, std::cref(A0), std::ref(y0));
        PRecursiveFFT(A1, y1);
    }
    std::vector<std::complex<double>> w (n / 2, w_n);
    std::exclusive_scan(w.begin(), w.end(), w.begin(), 1.0 + 0i, std::multiplies<>{});
    std::transform(se::unseq, w.begin(), w.end(), y1.begin(), y1.begin(), std::multiplies<>());
    std::transform(se::unseq, y0.begin(), y0.end(), y1.begin(), y.begin(), std::plus<>());
    std::transform(se::unseq, y0.begin(), y0.end(), y1.begin(), y.begin() + n / 2, std::minus<>());
}

int main() {
    constexpr std::size_t N = 1u << 5u;

    std::vector<double> v (N);
    std::iota(v.begin(), v.end(), 0);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle(v, gen);

    auto v2 = v;

    auto W = RecursiveFFT(v);
    for (const auto& w : W) {
        std::cout << w << ' ';
    }
    std::cout << '\n';

    std::vector<std::complex<double>> W2 (N);
    PRecursiveFFT(v2, W2);
    for (const auto& w : W2) {
        std::cout << w << ' ';
    }
    std::cout << '\n';
}