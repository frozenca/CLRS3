#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include <unordered_set>
#include <ranges>

std::mt19937 gen(std::random_device{}());

namespace sr = std::ranges;

int main() {
    constexpr size_t N = 100;
    constexpr size_t trials = 1000;
    std::uniform_int_distribution<> ind(0, N - 1);

    size_t total_count = 0;
    std::vector<size_t> v(N);
    std::iota(v.begin(), v.end(), 1);
    for (size_t t = 0; t < trials; t++) {
        std::unordered_set<size_t> s;
        while (s.size() < N) {
            total_count++;
            size_t new_index = ind(gen);
            if (v[new_index] == 1) {
                break;
            }
            s.insert(new_index);
        }
    }
    std::cout << "Expected value of indices of RANDOM-SEARCH : " << static_cast<double>(total_count) / static_cast<double>(trials) << '\n';

    constexpr size_t K = 3;
    for (size_t k = 0; k < K; k++) {
        v[k] = 1;
    }
    total_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::unordered_set<size_t> s;
        while (s.size() < N) {
            total_count++;
            size_t new_index = ind(gen);
            if (v[new_index] == 1) {
                break;
            }
            s.insert(new_index);
        }
    }
    std::cout << "Expected value of indices of RANDOM-SEARCH where " << K << " duplicates : "
    << static_cast<double>(total_count) / static_cast<double>(trials) << '\n';

    total_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::unordered_set<size_t> s;
        while (s.size() < N) {
            total_count++;
            size_t new_index = ind(gen);
            if (v[new_index] == N + 1) {
                break;
            }
            s.insert(new_index);
        }
    }
    std::cout << "Expected value of indices of RANDOM-SEARCH where zero desired elements : "
              << static_cast<double>(total_count) / static_cast<double>(trials) << '\n';

    total_count = 0;
    std::iota(v.begin(), v.end(), 1);
    for (size_t t = 0; t < trials; t++) {
        sr::shuffle(v, gen);
        for (size_t i = 0; i < N; i++) {
            if (v[i] == 1) {
                total_count += (i + 1);
                break;
            }
        }
    }
    std::cout << "Expected value of indices of DETERMINISTIC-SEARCH : " << static_cast<double>(total_count) / static_cast<double>(trials) << '\n';

    std::iota(v.begin(), v.end(), 1);
    for (size_t k = 0; k < K; k++) {
        v[k] = 1;
    }
    total_count = 0;
    for (size_t t = 0; t < trials; t++) {
        sr::shuffle(v, gen);
        for (size_t i = 0; i < N; i++) {
            if (v[i] == 1) {
                total_count += (i + 1);
                break;
            }
        }
    }
    std::cout << "Expected value of indices of DETERMINISTIC-SEARCH where " << K << " duplicates : "
              << static_cast<double>(total_count) / static_cast<double>(trials) << '\n';

    total_count = 0;
    for (size_t t = 0; t < trials; t++) {
        total_count += N;
    }
    std::cout << "Expected value of indices of DETERMINISTIC-SEARCH where zero desired elements : "
              << static_cast<double>(total_count) / static_cast<double>(trials) << '\n';

    std::iota(v.begin(), v.end(), 1);
    for (size_t k = 0; k < K; k++) {
        v[k] = 1;
    }
    total_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::shuffle(v.begin(), v.end(), gen);
        for (size_t i = 0; i < N; i++) {
            if (v[i] == 1) {
                total_count += (i + 1);
                break;
            }
        }
    }
    std::cout << "Expected value of indices of SCRAMBLED-SEARCH where " << K << " duplicates : "
              << static_cast<double>(total_count) / static_cast<double>(trials) << '\n';

}