#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <unordered_set>

std::mt19937 gen(std::random_device{}());

std::unordered_set<std::size_t> PollardRho(std::size_t n, std::size_t B) {
    std::size_t i = 1;
    std::uniform_int_distribution<> dist(0, n - 1);
    auto x = dist(gen);
    std::size_t y = x;
    std::size_t k = B;
    std::unordered_set<std::size_t> res;
    while (i <= n) {
        std::size_t P = 1;
        for (std::size_t k = 1; k <= B; k++) {
            x = (x * x - 1) % n;
            P *= (y - x);
        }
        i += B;
        auto d = std::gcd(P, n);
        if (d != 1 && d != n) {
            res.insert(d);
        }
        if (i >= k) {
            y = x;
            k *= 2;
        }
    }
    return res;
}

int main() {
    auto factors = PollardRho(1328789, 1);
    for (auto factor : factors) {
        std::cout << factor << ' ';
    }
    std::cout << '\n';
    auto factors2 = PollardRho(1328789, 10);
    for (auto factor : factors2) {
        std::cout << factor << ' ';
    }
    std::cout << '\n';

}