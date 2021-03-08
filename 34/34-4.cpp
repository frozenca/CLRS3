#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

namespace sr = std::ranges;

std::size_t getMaxProfit(const std::vector<std::size_t>& P,
                         const std::vector<std::size_t>& D,
                         const std::vector<std::size_t>& T) {
    const std::size_t n = P.size();
    assert(D.size() == n && T.size() == n);
    if (n == 0) {
        return 0;
    }

    auto max_t = *sr::max_element(D);
    std::vector<std::vector<std::size_t>> MP(max_t + 1, std::vector<std::size_t>(n));
    if (D[n - 1] >= T[n - 1]) {
        for (std::size_t t = 0; t <= D[n - 1] - T[n - 1]; t++) {
            MP[t][n - 1] = P[n - 1];
        }
    }
    for (std::size_t j = n - 2; j < n; j--) {
        for (std::size_t t = max_t; t < max_t + 1; t--) {
            if (t + T[j] <= D[j]) {
                MP[t][j] = MP[t + T[j]][j + 1] + P[j];
            } else {
                MP[t][j] = MP[t][j + 1];
            }
        }
    }
    return MP[0][0];
}

int main() {
    std::mt19937 gen(std::random_device{}());
    constexpr std::size_t n = 10;
    std::uniform_int_distribution time_dist (10, 100);
    std::uniform_int_distribution deadline_dist (10, 500);
    std::uniform_int_distribution profit_dist (1, 100);
    std::vector<std::size_t> P, D, T;
    P.reserve(n);
    D.reserve(n);
    T.reserve(n);
    for (std::size_t i = 0; i < n; i++) {
        P.push_back(profit_dist(gen));
        D.push_back(deadline_dist(gen));
        T.push_back(time_dist(gen));
    }
    std::cout << "Profits: ";
    for (auto p : P) {
        std::cout << p << ' ';
    }
    std::cout << '\n';
    std::cout << "Deadlines: ";
    for (auto d : D) {
        std::cout << d << ' ';
    }
    std::cout << '\n';
    std::cout << "Times: ";
    for (auto t : T) {
        std::cout << t << ' ';
    }
    std::cout << '\n';

    auto max_profit = getMaxProfit(P, D, T);
    std::cout << "Max profit: " << max_profit << '\n';
}