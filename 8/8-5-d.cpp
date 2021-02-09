#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

void averageSort(std::vector<int>& v, size_t k) {
    std::vector<std::vector<int>> V (k);
    for (size_t i = 0; i < v.size(); i++) {
        V[i % k].push_back(v[i]);
    }
    for (auto& V_ : V) {
        sr::sort(V_);
    }
    for (size_t m = 0; m < k; m++) {
        for (size_t i = 0; i < V[m].size(); i++) {
            v[m + i * k] = V[m][i];
        }
    }
    for (size_t i = 0; i < v.size() - k; i++) {
        assert(v[i] <= v[i + k]);
    }
}

int main() {
    constexpr size_t N = 500;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    sr::shuffle(v, gen);
    size_t k = 10;
    averageSort(v, k);

    for (size_t i = 0; i < N; i++) {
        std::cout << v[i] << ' ';
        if (i % k == (k - 1)) std::cout << '\n';
    }

}