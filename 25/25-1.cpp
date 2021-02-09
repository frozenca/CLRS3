#include <array>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <concepts>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <ranges>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace sr = std::ranges;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <std::size_t n, arithmetic T = double, bool is_undirected = false>
class Graph {
    std::vector<std::list<std::pair<std::size_t, T>>> adj;
    std::unordered_map<std::size_t, typename std::list<std::pair<std::size_t, T>>::iterator> edges;

public:
    Graph() : adj(n) {
        sr::fill(Tr, false);
    }

    void addEdge(std::size_t src, std::size_t dst, T weight) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, weight);
            edges[src * n + dst] = adj[src].begin();
            for (std::size_t i = 0; i < n; i++) {
                if (Tr[i * n + src] && !Tr[i * n + dst]) {
                    for (std::size_t j = 0; j < n; j++) {
                        if (Tr[dst * n + j]) {
                            Tr[i * n + j] = true;
                        }
                    }
                }
            }
        }
        if (is_undirected) {
            if (!edges.contains(dst * n + src)) {
                adj[dst].emplace_front(src, weight);
                edges[dst * n + src] = adj[dst].begin();
            }
            for (std::size_t i = 0; i < n; i++) {
                if (Tr[i * n + dst] && !Tr[i * n + src]) {
                    for (std::size_t j = 0; j < n; j++) {
                        if (Tr[src * n + j]) {
                            Tr[i * n + j] = true;
                        }
                    }
                }
            }
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

    using Edge = std::tuple<std::size_t, std::size_t, T>;

    std::array<bool, n * n> Tr;

    void TransitiveClosure() {
        sr::fill(Tr, false);
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                Tr[u * n + v] = true;
            }
            Tr[u * n + u] = true;
        }
        for (std::size_t k = 0; k < n; k++) {
            std::array<bool, n * n> Tr_;
            for (std::size_t i = 0; i < n; i++) {
                for (std::size_t j = 0; j < n; j++) {
                    Tr_[i * n + j] = Tr[i * n + j] || (Tr[i * n + k] && Tr[k * n + j]);
                }
            }
            Tr = Tr_;
        }
    }


};


int main() {
    Graph<4> g2;
    g2.TransitiveClosure();
    g2.addEdge(1, 2, 1);
    g2.addEdge(1, 3, 1);
    g2.addEdge(2, 1, 1);
    g2.addEdge(3, 0, 1);
    g2.addEdge(3, 2, 1);

    for (auto t : g2.Tr) {
        std::cout << t << ' ';
    }
    std::cout << '\n';

}