#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

std::mt19937 gen(std::random_device{}());

int main() {
    constexpr size_t N = 100;
    constexpr size_t k = 10;

    std::uniform_int_distribution<> dist(0, k);
    std::vector<size_t> v (N);
    for (auto& n : v) {
        n = dist(gen);
    }

    std::vector<size_t> B (v.size());
    std::vector<size_t> C (k + 1);
    for (auto n : v) {
        C[n]++;
    }
    for (size_t j = 1; j <= k; j++) {
        C[j] += C[j - 1];
    }

    std::cout << "Integers in [3, 5] : " << C[5] - C[2] << '\n';

}