#include <array>
#include <algorithm>
#include <cassert>
#include <cstddef>
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

    [[nodiscard]] std::pair<std::array<std::size_t, n * n>, std::array<T, n * n>>
    ExtendShortestPaths(const std::array<std::size_t, n * n>& Pi, const std::array<T, n * n>& L) const {
        std::array<T, n * n> L_;
        std::array<std::size_t, n * n> Pi_;
        sr::fill(L_, std::numeric_limits<T>::max());
        sr::fill(Pi_, -1);
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < n; j++) {
                for (std::size_t k = 0; k < n; k++) {
                    if (L_[i * n + j] > L[i * n + k] + W[k * n + j]) {
                        L_[i * n + j] = L[i * n + k] + W[k * n + j];
                        if (k != j) {
                            Pi_[i * n + j] = k;
                        } else {
                            Pi_[i * n + j] = Pi[i * n + j];
                        }
                    }
                }
            }
        }
        return {Pi_, L_};
    }

    [[nodiscard]] std::pair<std::array<std::size_t, n * n>, std::array<T, n * n>>
    SlowAllPairsShortestPaths() const {
        std::array<std::size_t, n * n> Pi;
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < n; j++) {
                if (W[i * n + j] == std::numeric_limits<T>::max()) {
                    Pi[i * n + j] = -1;
                } else {
                    Pi[i * n + j] = i;
                }
            }
        }
        auto L = W;
        for (std::size_t m = 2; m < n; m++) {
            std::tie(Pi, L) = ExtendShortestPaths(Pi, L);
        }
        return {Pi, L};
    }

};

int main() {
    Graph<5> g;
    g.addEdge(0, 1, 3);
    g.addEdge(0, 2, 8);
    g.addEdge(0, 4, -4);
    g.addEdge(1, 3, 1);
    g.addEdge(1, 4, 7);
    g.addEdge(2, 1, 4);
    g.addEdge(3, 0, 2);
    g.addEdge(3, 2, -5);
    g.addEdge(4, 3, 6);

    auto [Pi, L1] = g.SlowAllPairsShortestPaths();

    for (auto p : Pi) {
        std::cout << p << ' ';
    }
    std::cout << '\n';
    for (auto w : L1) {
        std::cout << w << ' ';
    }
    std::cout << '\n';


}