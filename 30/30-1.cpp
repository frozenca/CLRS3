#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

double Evaluate(const std::vector<double>& A, double x) {
    assert(!A.empty());
    double val = 0.0;
    for (std::size_t i = A.size() - 1; i < A.size(); --i) {
        val = val * x + A[i];
    }
    return val;
}

std::vector<double> PolynomialProductLg3(const std::vector<double>& A, const std::vector<double>& B) {
    std::size_t big_size = std::max(A.size(), B.size());
    std::size_t n = (1u << static_cast<std::size_t>(std::ceil(std::log2(big_size))));
    if (n == 1) {
        return std::vector<double>{A[0] * B[0]};
    }
    std::vector<double> A_poly = A;
    std::vector<double> B_poly = B;
    for (std::size_t k = 0; k < n - A.size(); k++) {
        A_poly.push_back(0);
    }
    for (std::size_t k = 0; k < n - B.size(); k++) {
        B_poly.push_back(0);
    }
    std::size_t half_n = n >> 1;
    std::vector<double> A_1 (half_n), A_2(half_n), B_1 (half_n), B_2(half_n), A_3(half_n), B_3(half_n);

    sr::copy(A_poly | srv::drop(half_n), A_1.begin());
    sr::copy(A_poly | srv::take(half_n), A_2.begin());
    sr::copy(B_poly | srv::drop(half_n), B_1.begin());
    sr::copy(B_poly | srv::take(half_n), B_2.begin());
    sr::transform(A_1, A_2, A_3.begin(), std::plus{});
    sr::transform(B_1, B_2, B_3.begin(), std::plus{});

    auto A_1B_1 = PolynomialProductLg3(A_1, B_1);
    auto A_2B_2 = PolynomialProductLg3(A_2, B_2);
    auto A_3B_3 = PolynomialProductLg3(A_3, B_3);

    std::size_t two_n = n << 1;
    std::vector<double> AB (two_n);
    sr::transform(AB | srv::drop(n), A_1B_1, AB.begin() + n, std::plus{});
    sr::transform(AB | srv::drop(half_n), A_3B_3, AB.begin() + half_n, std::plus{});
    sr::transform(AB | srv::drop(half_n), A_1B_1, AB.begin() + half_n, std::minus{});
    sr::transform(AB | srv::drop(half_n), A_2B_2, AB.begin() + half_n, std::minus{});
    sr::transform(AB, A_2B_2, AB.begin(), std::plus{});
    return AB;
}

uint64_t IntegerProductLg3(uint32_t a, uint32_t b) {
    constexpr std::size_t bits = 32;
    std::bitset<bits> A (a);
    std::bitset<bits> B (b);
    std::vector<double> A_poly(bits);
    std::vector<double> B_poly(bits);
    for (std::size_t i = 0; i < bits; i++) {
        A_poly[i] = A[i];
        B_poly[i] = B[i];
    }
    auto AB = PolynomialProductLg3(A_poly, B_poly);
    return static_cast<uint64_t>(Evaluate(AB, 2.0));
}

int main() {
    std::cout << IntegerProductLg3(53, 27) << ' ' << 53 * 27;
}