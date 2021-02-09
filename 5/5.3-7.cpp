#include <cassert>
#include <iostream>
#include <random>
#include <utility>
#include <unordered_set>

std::mt19937 gen(std::random_device{}());

std::unordered_set<size_t> randomSample(size_t m, size_t n) {
    assert(m <= n);
    if (m == 0) {
        std::unordered_set<size_t> res;
        return res;
    } else {
        std::unordered_set<size_t> res = randomSample(m - 1, n - 1);
        std::uniform_int_distribution<> dist(1, n);
        auto i = dist(gen);
        if (res.contains(i)) {
            res.insert(n);
        } else {
            res.insert(i);
        }
        return res;
    }
}

int main() {
    constexpr size_t N = 20;
    auto v = randomSample(8, N);
    for (auto n : v) {
        std::cout << n << ' ';
    }

}