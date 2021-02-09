#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>
#include <utility>

std::pair<std::vector<double>, double> LongDivision(const std::vector<double>& A, double x_0) {
    assert(!A.empty());
    auto A_ = A;
    std::size_t n = A.size();
    std::vector<double> q (n - 1);
    for (std::size_t k = n - 2; k < n; k--) {
        q[k] = A_[k + 1];
        A_[k] += x_0 * A_[k + 1];
    }
    auto r = A_[0];
    return {q, r};
}

std::vector<double> LagrangeFormula(const std::vector<std::pair<double, double>>& points) {
    assert(!points.empty());
    std::size_t n = points.size();
    std::vector<double> A(n + 1);
    A[0] = -points[0].first;
    A[1] = 1;
    for (std::size_t k = 1; k < n; k++) {
        for (std::size_t j = k; j <= k; j--) {
            A[j + 1] += A[j];
            A[j] *= -points[k].first;
        }
    }
    std::vector<double> A_(n);
    for (std::size_t k = 0; k < n; k++) {
        auto [A_k, _] = LongDivision(A, points[k].first);
        double p = 1;
        for (std::size_t j = 0; j < n; j++) {
            if (j == k) {
                continue;
            }
            p *= (points[k].first - points[j].first);
        }
        for (std::size_t j = 0; j < n; j++) {
            A_k[j] *= points[k].second / p;
        }
        for (std::size_t j = 0; j < n; j++) {
            A_[j] += A_k[j];
        }
    }
    return A_;
}

int main() {
    std::vector<std::pair<double, double>> points {{0, 0}, {1, 1}, {2, 4}};
    auto A = LagrangeFormula(points);
    for (auto a_n : A) {
        std::cout << a_n << ' ';
    }
    std::cout << '\n';

}