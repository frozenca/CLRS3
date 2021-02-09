#include <array>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cmath>
#include <concepts>
#include <iostream>
#include <limits>
#include <ranges>
#include <type_traits>

namespace sr = std::ranges;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <std::size_t n, arithmetic T = double, bool undirected = false>
class Graph {
    std::array<T, n * n> W;

public:
    Graph() {
        sr::fill(W, std::numeric_limits<T>::max());
        for (std::size_t i = 0; i < n; i++) {
            W[i * n + i] = T{0};
        }
    }

    void addEdge(std::size_t src, std::size_t dst, T weight) {
        assert(src < n && dst < n);
        W[src * n + dst] = weight;
        if (undirected) {
            W[dst * n + src] = weight;
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        addEdge(src, dst, std::numeric_limits<T>::max());
    }

    [[nodiscard]] T getWeight(std::size_t src, std::size_t dst) const {
        return W[src * n + dst];
    }

    [[nodiscard]] std::array<T, n * n> ExtendShortestPaths(const std::array<T, n * n>& L1,
                                                           const std::array<T, n * n>& L2) const {
        std::array<T, n * n> L_;
        sr::fill(L_, std::numeric_limits<T>::max());
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < n; j++) {
                for (std::size_t k = 0; k < n; k++) {
                    L_[i * n + j] = std::min(L_[i * n + j], L1[i * n + k] + L2[k * n + j]);
                }
            }
        }
        return L_;
    }

    [[nodiscard]] bool hasDiagonalNegative(const std::array<T, n * n>& L) const {
        for (std::size_t i = 0; i < n; i++) {
            if (L[i * n + i] < 0) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::size_t FindMinLengthNegWeightCycle() const {
        std::vector<std::array<T, n * n>> Ls;
        auto L = W;
        std::size_t m = 1;
        while (m <= n && !hasDiagonalNegative(L)) {
            Ls.push_back(L);
            L = ExtendShortestPaths(L, L);
            m *= 2;
        }
        if (m > n && !hasDiagonalNegative(L)) {
            return -1;
        } else if (m <= 2) {
            return m;
        } else {
            std::size_t low = m / 2;
            std::size_t high = m;
            std::size_t d = m / 4;
            while (d >= 1) {
                std::size_t s = low + d;
                auto L_s = ExtendShortestPaths(Ls[std::floor(std::log2(low))], Ls[std::floor(std::log2(d))]);
                if (hasDiagonalNegative(L_s)) {
                    high = s;
                } else {
                    low = s;
                    d = d / 2;
                }
            }
            return high;
        }
    }

};


int main() {
    Graph<5> g;
    g.addEdge(0, 1, 3);
    g.addEdge(0, 2, 8);
    g.addEdge(0, 4, -4);
    g.addEdge(1, 3, 1);
    g.addEdge(1, 4, -20);
    g.addEdge(2, 1, 4);
    g.addEdge(3, 0, 2);
    g.addEdge(3, 2, -5);
    g.addEdge(4, 3, 6);

    std::cout << g.FindMinLengthNegWeightCycle();


}