#include <array>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <concepts>
#include <iostream>
#include <limits>
#include <ranges>
#include <type_traits>
#include <utility>

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

    [[nodiscard]] std::pair<std::array<std::size_t, n * n>, std::array<T, n * n>> FloydWarshall() const {
        std::array<std::size_t, n * n> Pi;
        auto D = W;
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < n; j++) {
                if (i != j && D[i * n + j] != std::numeric_limits<T>::max()) {
                    Pi[i * n + j] = i;
                } else {
                    Pi[i * n + j] = -1;
                }
            }
        }
        for (std::size_t k = 0; k < n; k++) {
            std::array<std::size_t, n * n> Pi_;
            for (std::size_t i = 0; i < n; i++) {
                for (std::size_t j = 0; j < n; j++) {
                    if (D[i * n + j] <= D[i * n + k] + D[k * n + j]) {
                        D[i * n + j] = D[i * n + j];
                        Pi_[i * n + j] = Pi[i * n + j];
                    } else {
                        D[i * n + j] = D[i * n + k] + D[k * n + j];
                        Pi_[i * n + j] = k;
                    }
                }
            }
            std::swap(Pi, Pi_);
        }
        return {Pi, D};
    }

    void PrintAllPairsShortestPath(const std::array<std::size_t, n * n>& Pi, std::size_t i, std::size_t j) const {
        if (i == j) {
            std::cout << i << ' ';
        } else if (Pi[i * n + j] == -1) {
            std::cout << "No path from " << i << " to " << j << " exists\n";
        } else {
            PrintAllPairsShortestPath(Pi, i, Pi[i * n + j]);
            std::cout << Pi[i * n + j] << ' ';
            PrintAllPairsShortestPath(Pi, Pi[i * n + j], j);
        }
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

    auto [Pi, D] = g.FloydWarshall();
    for (auto p : Pi) {
        std::cout << p << ' ';
    }
    std::cout << '\n';
    for (auto w : D) {
        std::cout << w << ' ';
    }
    std::cout << '\n';




}