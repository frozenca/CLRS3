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
    std::swap(v[0], v[dist(gen)]);
    for (size_t i = 1; i < N; i++) {
        std::uniform_int_distribution<> dist_local(i, N - 1);
        std::swap(v[i], v[dist_local(gen)]);
    }
    for (auto n : v) {
        std::cout << n << ' ';
    }

}