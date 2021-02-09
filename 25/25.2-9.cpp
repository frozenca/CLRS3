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

    using Edge = std::tuple<std::size_t, std::size_t, T>;

    [[nodiscard]] std::array<bool, n * n> TransitiveClosureNaive() const {
        std::array<bool, n * n> Tr;
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
            std::swap(Tr, Tr_);
        }
        return Tr;
    }

    [[nodiscard]] Graph getTranspose() const {
        Graph g;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                g.addEdge(v, u, w);
            }
        }
        return g;
    }

    [[nodiscard]] std::tuple<std::unordered_map<std::size_t, std::size_t>,
            std::unordered_map<std::size_t, std::unordered_set<std::size_t>>,
                    std::size_t> getSCC() const {
        std::stack<std::size_t> S;
        std::list<std::size_t> toVisit;
        std::vector<int> visited (n);
        for (std::size_t u = 0; u < n; u++) {
            if (!visited[u]) {
                S.push(u);
                while (!S.empty()) {
                    auto v = S.top();
                    visited[v] = true;
                    bool finished = true;
                    for (const auto& [next, w] : adj[v]) {
                        if (!visited[next]) {
                            S.push(next);
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                        toVisit.push_front(v);
                    }
                }
            }
        }

        auto g = getTranspose();

        std::unordered_map<std::size_t, std::size_t> OrigToSCC;
        std::unordered_map<std::size_t, std::unordered_set<std::size_t>> SCCToOrigs;
        std::size_t SCC_index = 0;

        sr::fill(visited, 0);
        for (auto u : toVisit) {
            if (!visited[u]) {
                S.push(u);
                while (!S.empty()) {
                    auto v = S.top();
                    OrigToSCC[v] = SCC_index;
                    SCCToOrigs[SCC_index].insert(v);
                    visited[v] = true;
                    bool finished = true;
                    for (const auto& [next, w] : g.adj[v]) {
                        if (!visited[next]) {
                            S.push(next);
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                    }
                }
                SCC_index++;
            }
        }
        return {OrigToSCC, SCCToOrigs, SCC_index};
    }

    [[nodiscard]] std::array<bool, n * n> TransitiveClosure() const {
        auto [OrigToSCC, SCCToOrigs, num_SCC] = getSCC();
        Graph SCCGraph;

        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                SCCGraph.addEdge(OrigToSCC[u], OrigToSCC[v], 1);
            }
        }

        auto Tr_SCC = SCCGraph.TransitiveClosureNaive();

        std::array<bool, n * n> Tr;
        sr::fill(Tr, false);

        for (std::size_t u_scc = 0; u_scc < num_SCC; u_scc++) {
            auto& scc_u = SCCToOrigs[u_scc];
            for (std::size_t v_scc = 0; v_scc < num_SCC; v_scc++) {
                auto& scc_v = SCCToOrigs[v_scc];
                if (Tr_SCC[u_scc * n + v_scc]) {
                    for (auto vertex_scc_u : scc_u) {
                        for (auto vertex_scc_v : scc_v) {
                            Tr[vertex_scc_u * n + vertex_scc_v] = true;
                        }
                    }
                }
            }
            for (auto vertex_scc_u : scc_u) {
                for (auto vertex_scc_u2 : scc_u) {
                    Tr[vertex_scc_u * n + vertex_scc_u2] = true;
                }
            }
        }

        return Tr;
    }


};


int main() {
    Graph<4> g2;
    g2.addEdge(1, 2, 1);
    g2.addEdge(1, 3, 1);
    g2.addEdge(2, 1, 1);
    g2.addEdge(3, 0, 1);
    g2.addEdge(3, 2, 1);

    auto Tr = g2.TransitiveClosure();
    for (auto t : Tr) {
        std::cout << t << ' ';
    }
    std::cout << '\n';

}