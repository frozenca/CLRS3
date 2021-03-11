#include <algorithm>
#include <cassert>
#include <ranges>
#include <iostream>
#include <vector>
#include <utility>

namespace sr = std::ranges;

std::size_t Knapsack(const std::vector<std::pair<std::size_t, std::size_t>>& items, std::size_t W) {
    assert(W > 0 && !items.empty());
    const std::size_t n = items.size();
    std::vector<std::vector<std::size_t>> m (n + 1, std::vector<std::size_t>(W + 1));
    for (std::size_t i = 1; i <= n; i++) {
        for (std::size_t j = 0; j <= W; j++) {
            if (items[i - 1].first > j) {
                m[i][j] = m[i - 1][j];
            } else {
                m[i][j] = std::max(m[i - 1][j], m[i - 1][j - items[i - 1].first] + items[i - 1].second);
            }
        }
    }
    return m[n][W];
}

std::size_t ApproxKnapsack(std::vector<std::pair<std::size_t, std::size_t>>& items, std::size_t W) {
    assert(W > 0 && !items.empty());
    assert(sr::all_of(items, [](const auto& vw){return vw.first > 0.0 && vw.second > 0.0;}));
    auto comp = [](const auto& vw1, const auto& vw2) {
        return vw1.second / (1.0 * vw1.first) > vw2.second / (1.0 * vw2.second);
    };
    sr::sort(items, comp);
    const std::size_t n = items.size();
    std::size_t max_val = 0;
    for (std::size_t i = 0; i < n; i++) {
        const auto& [w_i, v_i] = items[i];
        auto w = w_i;
        auto v = v_i;
        for (std::size_t j = 0; j < n; j++) {
            const auto& [w_j, v_j] = items[j];
            if (j != i && w + w_j <= W) {
                w += w_j;
                v += v_j;
            }
        }
        max_val = std::max(v, max_val);
    }
    return max_val;
}

int main() {
    std::vector<std::pair<std::size_t, std::size_t>> items {
            {23, 505}, {26, 352}, {20, 458}, {18, 220},
            {32, 354}, {27, 414}, {29, 498}, {26, 545},
            {30, 473}, {27, 543},
    };

    constexpr std::size_t W = 67;

    auto knapsack = Knapsack(items, W);
    std::cout << knapsack << '\n';
    auto approx_knapsack = ApproxKnapsack(items, W);
    std::cout << approx_knapsack << '\n';
}
