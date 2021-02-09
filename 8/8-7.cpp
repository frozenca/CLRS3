#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

int main() {
    constexpr size_t R = 14;
    constexpr size_t S = 3;
    constexpr size_t N = R * S;
    static_assert(R % 2 == 0);
    std::vector<std::vector<size_t>> A (S, std::vector<size_t>(R));
    std::vector<size_t> B (N);
    std::iota(B.begin(), B.end(), 1);
    sr::shuffle(B, gen);
    for (size_t s = 0; s < S; s++) {
        for (size_t r = 0; r < R; r++) {
            A[s][r] = B[s * R + r];
        }
    }
    for (const auto& row : A) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
    // step 1
    for (auto& row : A) {
        sr::sort(row);
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // step 2
    std::vector<std::vector<size_t>> C (S, std::vector<size_t>(R));
    for (size_t i = 0; i < N; i++) {
        C[i % S][i / S] = A[i / R][i % R];
    }
    A = C;
    for (const auto& row : A) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // step 3
    for (auto& row : A) {
        sr::sort(row);
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // step 4
    for (size_t i = 0; i < N; i++) {
        C[i / R][i % R] = A[i % S][i / S];
    }
    A = C;
    for (const auto& row : A) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // step 5
    for (auto& row : A) {
        sr::sort(row);
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // step 6
    C.clear();
    C.resize(S + 1, std::vector<size_t>(R));
    for (size_t s = 0; s < S; s++) {
        for (size_t r = 0; r < R / 2; r++) {
            C[s][r + R / 2] = A[s][r];
        }
        for (size_t r = R / 2; r < R; r++) {
            C[s + 1][r - R / 2] = A[s][r];
        }
    }
    C.back().resize(R / 2);
    A = C;
    for (const auto& row : A) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // step 7
    for (auto& row : A) {
        sr::sort(row);
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // step 8
    C.clear();
    C.resize(S, std::vector<size_t>(R));
    for (size_t s = 0; s < S; s++) {
        for (size_t r = R / 2; r < R; r++) {
            C[s][r - R / 2] = A[s][r];
        }
        for (size_t r = 0; r < R / 2; r++) {
            C[s][r + R / 2] = A[s + 1][r];
        }
    }
    A = C;
    for (const auto& row : A) {
        for (auto n : row) {
            std::cout << n << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
    
}