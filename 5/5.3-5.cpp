#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

int main() {
    constexpr size_t N = 20;
    constexpr size_t trials = 1000;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(1, std::pow(N, 3));
    size_t unique_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::vector<int> v(N);
        for (size_t i = 0; i < N - 1; i++) {
            v[i] = dist(gen);
        }
        sr::sort(v);
        v.erase(sr::unique(v), v.end());
        if (v.size() == N) {
            unique_count++;
        }
    }
    std::cout << "Probability that elems are unique : " << static_cast<double>(unique_count) / static_cast<double>(trials) << '\n';
    std::cout << "1 - (1 / N) : " << 1.0 - 1.0 / N << '\n';

}