#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

double InventoryPlanning(size_t n, const std::vector<size_t>& d, double c, size_t m, const std::vector<double>& h) {
    auto D = std::accumulate(d.begin(), d.end(), 0ul);
    assert(h.size() == D);
    std::vector<std::vector<double>> cost (n, std::vector<double>(D + 1));
    for (size_t s = 0; s <= D; s++) {
        auto f = std::max(static_cast<int>(d[n - 1]) - static_cast<int>(s), 0);
        cost[n - 1][s] = c * std::max(f - static_cast<int>(m), 0) + h[s + f - d[n]];
    }
    auto U = d[n - 1];
    for (size_t k = n - 2; k < n; k--) {
        U += d[k];
        for (size_t s = 0; s <= D; s++) {
            cost[k][s] = std::numeric_limits<double>::max();
            for (int f = std::max(static_cast<int>(d[k]) - static_cast<int>(s), 0); f <= U - s; f++) {
                auto v = cost[k + 1][s + f - d[k]] + c * std::max(f - static_cast<int>(m), 0) + h[s + f - d[k]];
                if (v < cost[k][s]) {
                    cost[k][s] = v;
                }
            }
        }
    }

    return cost[0][0];
}


int main() {

}