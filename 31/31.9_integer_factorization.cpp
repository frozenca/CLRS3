#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <unordered_set>

std::mt19937 gen(std::random_device{}());

std::unordered_set<std::size_t> PollardRho(std::size_t n) {
    std::size_t i = 1;
    std::uniform_int_distribution<> dist(0, n - 1);
    auto x = dist(gen);
    std::size_t y = x;
    std::size_t k = 2;
    std::unordered_set<std::size_t> res;
    while (i <= n) {
        i++;
        x = (x * x - 1) % n;
        auto d = std::gcd(y - x, n);
        if (d != 1 && d != n) {
            res.insert(d);
        }
        if (i == k) {
            y = x;
            k *= 2;
        }
    }
    return res;
}

int main() {
    auto factors = PollardRho(1328789);
    for (auto factor : factors) {
        std::cout << factor << ' ';
    }
    std::cout << '\n';

}