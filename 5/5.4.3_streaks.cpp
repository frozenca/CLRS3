#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>

std::mt19937 gen(std::random_device{}());

int main() {
    constexpr size_t N = 100;
    constexpr size_t trials = 1000;
    std::bernoulli_distribution coin(0.5);
    size_t longest_streak_count = 0;
    for (size_t t = 0; t < trials; t++) {
        size_t longest_streak = 0;
        size_t current_streak = 0;
        for (size_t i = 0; i < N; i++) {
            if (coin(gen)) {
                current_streak++;
            } else {
                longest_streak = std::max(longest_streak, current_streak);
                current_streak = 0;
            }
        }
        longest_streak = std::max(longest_streak, current_streak);
        longest_streak_count += longest_streak;
    }

    std::cout << "Expected number of longest streak : " << static_cast<double>(longest_streak_count) /
            static_cast<double>(trials) << '\n';
    std::cout << "ln N : " << std::log(N);

}