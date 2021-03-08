#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <ranges>
#include <numeric>
#include <vector>
#include <utility>

namespace sr = std::ranges;

std::pair<std::vector<std::size_t>, std::vector<std::size_t>> DivideEvenly(std::vector<std::size_t>& coins) {
    if (coins.empty()) {
        return {{}, {}};
    }
    sr::sort(coins, std::greater<>());
    std::size_t p = coins.size() - 1;
    std::vector<std::size_t> b;
    std::vector<std::size_t> c;
    while (p > 0) {
        auto s = 0;
        std::size_t j = p - 1;
        while (j < p && s < coins[p]) {
            s += coins[j];
            j--;
        }
        if (s != coins[p]) {
            return {{}, {}};
        } else {
            b.push_back(coins[p]);
            std::copy(coins.begin() + j + 1, coins.begin() + p, std::back_inserter(c));
        }
        p = j;
    }
    return {std::move(b), std::move(c)};
}

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> num_coins (1, 20);
    std::size_t num_denoms = 5;
    std::vector<std::size_t> denoms (10);
    std::iota(denoms.begin(), denoms.end(), 0);
    sr::shuffle(denoms, gen);
    std::vector<std::size_t> coins;
    for (std::size_t i = 0; i < num_denoms; i++) {
        std::size_t num_coin = num_coins(gen);
        for (std::size_t j = 0; j < num_coin; j++) {
            coins.push_back(1u << denoms[i]);
        }
    }

    auto [b, c] = DivideEvenly(coins);
    for (auto b_coin : b) {
        std::cout << b_coin << ' ';
    }
    std::cout << '\n';
    for (auto c_coin : c) {
        std::cout << c_coin << ' ';
    }
    std::cout << '\n';
}