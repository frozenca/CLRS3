#include <cassert>
#include <iostream>
#include <random>
#include <numeric>
#include <utility>
#include <vector>

int main() {
    constexpr size_t N = 20;
    std::vector<int> v (N);
    std::mt19937 gen(std::random_device{}());
    std::iota(v.begin(), v.end(), 1);
    std::uniform_int_distribution<> dist(0, N - 1);
    for (size_t i = 0; i < N; i++) {
        std::swap(v[i], v[dist(gen)]);
    }
    for (auto n : v) {
        std::cout << n << ' ';
    }

}