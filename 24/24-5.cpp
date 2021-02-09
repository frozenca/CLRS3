#include <algorithm>
#include <cassert>
#include <concepts>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <queue>
#include <random>
#include <stack>
#include <utility>
#include <vector>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

constexpr std::size_t V = 1'000;

std::mt19937 gen(std::random_device{}());
std::uniform_real_distribution<> dist(-100.0, 100.0);
std::bernoulli_distribution d(0.01);

template <std::size_t n, arithmetic T = double, bool undirected = false>
class Graph {
    std::vector<std::list<std::pair<std::size_t, T>>> adj;
    std::unordered_map<std::size_t, typename std::list<std::pair<std::size_t, T>>::iterator> edges;

public:
    Graph() : adj(n) {
    }

    void addEdge(std::size_t src, std::size_t dst, T weight) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, weight);
            edges[src * n + dst] = adj[src].begin();
        }
        if (undirected) {
            if (!edges.contains(dst * n + src)) {
                adj[dst].emplace_front(src, weight);
                edges[dst * n + src] = adj[dst].begin();
            }
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        if (undirected) {
            adj[dst].erase(edges[dst * n + src]);
            edges.erase(dst * n + src);
        }
    }

    [[nodiscard]] T getWeight(std::size_t src, std::size_t dst) const {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return T {0};
        } else {
            return edges.at(src * n + dst)->second;
        }
    }

    [[nodiscard]] T getMinimumMeanCycleWeight(std::size_t source) const {
        std::vector<std::vector<T>> delta(n + 1, std::vector<T>(n, std::numeric_limits<T>::max()));

        for (std::size_t i = 1; i <= n; i++) {
            for (std::size_t u = 0; u < n; u++) {
                for (const auto& [v, w] : adj[u]) {
                    delta[i][v] = std::min(delta[i][v], ((i == 1) ? 0 : delta[i - 1][u]) + w);
                }
            }
        }
        std::vector<T> fractions (n, std::numeric_limits<T>::lowest());

        for (std::size_t v = 0; v < n; v++) {
            for (std::size_t k = 0; k < n; k++) {
                fractions[v] = std::max(fractions[v], (delta[n][v] - delta[k][v]) / (n - k));
            }
        }
        return *sr::min_element(fractions);
    }
};

int main() {
    Graph<V> g;
    for (std::size_t i = 0; i < V; i++) {
        for (std::size_t j = i + 1; j < V; j++) {
            if (d(gen)) {
                auto weight = dist(gen);
                g.addEdge(i, j, weight);
            }
            if (d(gen)) {
                auto weight = dist(gen);
                g.addEdge(j, i, weight);
            }
        }
    }

    std::cout << "Minimum mean cycle weight is : ";
    std::cout << g.getMinimumMeanCycleWeight(0);
}