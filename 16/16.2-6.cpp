#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <ranges>

namespace sr = std::ranges;

double fractionalKnapsack(std::vector<std::pair<double, double>>& items, double W) {
    assert(W > 0.0);
    assert(sr::all_of(items, [](auto& p) {return p.first > 0.0 && p.second > 0.0;}));
    sr::sort(items, [](auto& p1, auto& p2) {return p1.first / p1.second > p2.first / p2.second;});
    double currWeight = 0.0;
    double finalValue = 0.0;
    for (const auto& [value, weight] : items) {
        if (currWeight + weight <= W) {
            currWeight += weight;
            finalValue += value;
        } else {
            auto remain = W - weight;
            finalValue += remain * value / weight;
        }
    }
    return finalValue;
}

int main() {
}
