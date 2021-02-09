#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>
#include <utility>

template <typename T>
std::pair<std::vector<T>, T> LongDivision(const std::vector<T>& A, const T& x_0) {
    assert(!A.empty());
    auto A_ = A;
    std::size_t n = A.size();
    std::vector<T> q (n - 1);
    for (std::size_t k = n - 2; k < n; k--) {
        q[k] = A_[k + 1];
        A_[k] += x_0 * A_[k + 1];
    }
    auto r = A_[0];
    return {q, r};
}

int main() {
    std::vector<double> A {-4, 0, -2, 1};
    auto [q, r] = LongDivision(A, 3.0);
    for (auto q_ : q) {
        std::cout << q_ << ' ';
    }
    std::cout << '\n';
    std::cout << r << '\n';


}