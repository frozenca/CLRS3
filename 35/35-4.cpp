#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace sr = std::ranges;

class Graph {
    const std::size_t n;
    std::vector<std::list<std::size_t>> adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> edges;
public:
    Graph(std::size_t n) : n {n}, adj(n) {}

    bool hasEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        return edges.contains(src * n + dst);
    }

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].push_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
        if (!edges.contains(dst * n + src)) {
            adj[dst].push_front(src);
            edges[dst * n + src] = adj[dst].begin();
        }
    }

    [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> GreedyMatching() {
        std::vector<std::pair<std::size_t, std::size_t>> S;
        std::unordered_set<std::size_t> E;
        for (const auto& [e_val, _] : edges) {
            E.insert(e_val);
        }
        for (const auto& [e_val, _] : edges) {
            if (E.contains(e_val)) {
                auto u = e_val / n;
                auto v = e_val % n;
                for (auto u_next : adj[u]) {
                    if (u_next != v) {
                        E.erase(u * n + u_next);
                        E.erase(u_next * n + u);
                    }
                }
                for (auto v_next : adj[v]) {
                    if (v_next != u) {
                        E.erase(v * n + v_next);
                        E.erase(v_next * n + v);
                    }
                }
                E.erase(v * n + u);
                S.emplace_back(u, v);
            }
        }
        return S;
    }
};

int main() {
    std::mt19937 gen(std::random_device{}());
    const std::size_t n = 100;
    std::uniform_int_distribution<> dist(0, n - 1);
    Graph g(n);
    for (std::size_t i = 0; i < 10 * n; i++) {
        auto u = dist(gen);
        auto v = dist(gen);
        if (u != v) {
            g.addEdge(u, v);
        }
    }

    auto GM = g.GreedyMatching();

    for (const auto& [u, v] : GM) {
        std::cout << u << ' ' << v << '\n';
    }
    std::cout << '\n';
}