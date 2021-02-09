#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include <ranges>

std::mt19937 gen(std::random_device{}());

namespace sr = std::ranges;

int main() {
    constexpr size_t N = 100;
    constexpr size_t trials = 10000;
    std::vector<size_t> v (N);
    std::iota(v.begin(), v.end(), 1);

    for (size_t K = 1; K < N; K++) {
        size_t total_best_score = 0;
        for (size_t t = 0; t < trials; t++) {
            auto u = v;
            sr::shuffle(u, gen);
            size_t best_score = 0;
            for (size_t i = 0; i < K; i++) {
                best_score = std::max(best_score, u[i]);
            }
            for (size_t i = K; i < N; i++) {
                if (u[i] > best_score) {
                    total_best_score += u[i];
                    break;
                }
            }
        }
        std::cout << "Average best score with K " << K << " and N " << N << " : "
                  << static_cast<double>(total_best_score) / static_cast<double>(trials) << '\n';
    }
}