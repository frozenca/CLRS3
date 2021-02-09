#include <cstddef>
#include <iostream>
#include <tuple>
#include <vector>

std::tuple<std::size_t, int, int> ExtendedEuclid(std::size_t a, std::size_t b) {
    if (!b) {
        return {a, 1, 0};
    } else {
        auto [d_, x_, y_] = ExtendedEuclid(b, a % b);
        std::size_t d = d_;
        int x = y_;
        int y = x_ - static_cast<int>(a / b) * y_;
        return {d, x, y};
    }
}

std::size_t mod(int a, std::size_t n) {
    if (a >= 0) {
        return a % n;
    } else {
        return n - ((-a) % n);
    }
}

std::vector<std::size_t> ModularLinearEquationSolver(std::size_t a, int b, std::size_t n) {
    auto [d, x_, y_] = ExtendedEuclid(a, n);
    if ((b % d) == 0) {
        std::size_t x_0 = mod(x_ * b / static_cast<int>(d), n);
        std::vector<std::size_t> res;
        for (std::size_t i = 0; i < d; i++) {
            res.push_back((x_0 + i * (n / d)) % n);
        }
        return res;
    } else {
        return {};
    }
}

int main() {
    auto ans = ModularLinearEquationSolver(14, 30, 100);
    for (auto x : ans) {
        std::cout << x << ' ';
    }
    std::cout << '\n';

}