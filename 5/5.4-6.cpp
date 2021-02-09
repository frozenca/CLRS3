#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

std::mt19937 gen(std::random_device{}());

int main() {
    constexpr size_t N = 100;
    constexpr size_t trials = 1000;
    std::uniform_int_distribution<> toss(0, N - 1);
    size_t empty_bin_count = 0;
    size_t one_bin_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::vector<int> bin(N);
        for (size_t i = 0; i < N; i++) {
            bin[toss(gen)] += 1;
        }
        for (auto balls : bin) {
            if (balls == 0) {
                empty_bin_count++;
            } else if (balls == 1) {
                one_bin_count++;
            }
        }
    }

    std::cout << "Expected number of empty bins : " << static_cast<double>(empty_bin_count) /
                                                   static_cast<double>(trials) << '\n';
    std::cout << "n ((n-1)/n)^n : " << (N * std::pow(static_cast<double>(N - 1) / N, N)) << '\n';
    std::cout << "Expected number of bins with exactly one ball : " << static_cast<double>(one_bin_count) /
                                                       static_cast<double>(trials) << '\n';
    std::cout << "n ((n-1)/n)^(n-1) : " << (N * std::pow(static_cast<double>(N - 1) / N, N - 1)) << '\n';

}