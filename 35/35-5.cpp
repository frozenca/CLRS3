#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>
#include <queue>

namespace sr = std::ranges;

std::vector<std::vector<std::size_t>> GreedySchedule(std::size_t m, const std::vector<double>& p) {
    assert(sr::all_of(p, [](auto num){return num > 0.0;}));
    std::priority_queue<std::pair<double, std::size_t>, std::vector<std::pair<double, std::size_t>>, std::greater<>> Q;
    const std::size_t n = p.size();
    for (std::size_t i = 0; i < m; i++) {
        Q.emplace(0, i);
    }
    std::vector<std::vector<std::size_t>> A (m);
    for (std::size_t i = 0; i < n; i++) {
        auto [f_k, k] = Q.top();
        Q.pop();
        f_k += p[i];
        A[k].push_back(i);
        Q.emplace(f_k, k);
    }
    return A;
}

int main() {
    constexpr std::size_t m = 5;
    constexpr std::size_t n = 100;
    std::vector<double> p;

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(1.0, 100.0);
    p.reserve(n);
    for (std::size_t i = 0; i < n; i++) {
        p.push_back(dist(gen));
    }

    for (auto p_val : p) {
        std::cout << p_val << ' ';
    }
    std::cout << '\n';

    auto GS = GreedySchedule(m, p);
    for (const auto& gs : GS) {
        for (auto idx : gs) {
            std::cout << idx << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

}