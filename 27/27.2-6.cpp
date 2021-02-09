#include <array>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <concepts>
#include <execution>
#include <iostream>
#include <limits>
#include <numeric>
#include <ranges>
#include <type_traits>
#include <utility>

namespace se = std::execution;
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

    [[nodiscard]] std::array<T, n * n> PFloydWarshall() const {
        std::array<T, n * n> D;
        std::copy(se::unseq, W.begin(), W.end(), D.begin());
        for (std::size_t k = 0; k < n; k++) {
            std::array<std::size_t, n> v;
            std::iota(v.begin(), v.end(), 0);
            std::for_each(se::unseq, v.begin(), v.end(), [&D, k](auto i) {
                std::array<std::size_t, n> w;
                std::iota(w.begin(), w.end(), 0);
                std::for_each(se::unseq, w.begin(), w.end(), [&D, i, k](auto j) {
                    D[i * n + j] = std::min(D[i * n + j], D[i * n + k] + D[k * n + j]);
                });
            });
        }
        return D;
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

    auto D = g.PFloydWarshall();
    for (auto w : D) {
        std::cout << w << ' ';
    }
    std::cout << '\n';




}