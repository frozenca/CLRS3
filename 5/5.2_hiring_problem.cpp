#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <algorithm>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

size_t hiringCost(const std::vector<int>& v) {
    int best = 0;
    size_t count = 0;
    for (auto n : v) {
        if (n > best) {
            best = n;
            count++;
        }
    }
    return count;
}

int main() {
    constexpr size_t N = 1000;
    constexpr size_t trials = 1000;
    size_t cost = 0;

    for (size_t i = 0; i < trials; i++) {
        std::vector<int> candidates(N);
        std::iota(candidates.begin(), candidates.end(), 1);
        sr::shuffle(candidates, gen);
        cost += hiringCost(candidates);
    }

    std::cout << "Average hiring cost : " << static_cast<double>(cost) / static_cast<double>(trials) << '\n';
    std::cout << "ln n : " << std::log(N) << '\n';
}