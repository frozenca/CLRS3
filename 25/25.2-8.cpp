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
#include <utility>
#include <vector>

namespace sr = std::ranges;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <std::size_t n, arithmetic T = double, bool undirected = false>
class GraphAdj {
    std::vector<std::list<std::pair<std::size_t, T>>> adj;
    std::unordered_map<std::size_t, typename std::list<std::pair<std::size_t, T>>::iterator> edges;

public:
    GraphAdj() : adj(n) {
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

    std::vector<T> DijkstraArray(std::size_t s, std::size_t E) {
        std::vector<std::list<std::size_t>> Q (E + 2);

        std::size_t min_val = 0;

        std::vector<T> dists (n, E + 1);
        dists[s] = T{0};
        Q[0].push_back(s);
        while (!sr::all_of(Q, [](const auto& l){return l.empty();})) {
            min_val = std::distance(Q.begin(),
                                    std::find_if(Q.begin() + min_val, Q.end(), [](const auto &l) { return !l.empty(); }));
            auto u = Q[min_val].front();
            Q[min_val].pop_front();
            for (const auto& [v, w] : adj[u]) {
                if (dists[v] > min_val + w) {
                    dists[v] = min_val + w;
                    Q[dists[v]].push_back(v);
                }
            }
        }
        return dists;
    }


};

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

    [[nodiscard]] std::array<bool, n * n> TransitiveClosure() const {
        std::size_t E = 0;
        GraphAdj<n, T> g;
        for (std::size_t u = 0; u < n; u++) {
            for (std::size_t v = 0; v < n; v++) {
                if (W[u * n + v] != std::numeric_limits<T>::max()) {
                    g.addEdge(u, v, 1);
                    E++;
                }
            }
        }
        std::array<bool, n * n> Tr;
        for (std::size_t u = 0; u < n; u++) {
            auto dists = g.DijkstraArray(u, E);
            for (std::size_t v = 0; v < n; v++) {
                if (dists[v] != E + 1) {
                    Tr[u * n + v] = true;
                } else {
                    Tr[u * n + v] = false;
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