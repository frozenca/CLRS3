#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <tuple>
#include <concepts>
#include <limits>
#include <ranges>
#include <stack>
#include <random>
#include <cmath>

namespace sr = std::ranges;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <std::size_t n, arithmetic T = double, bool undirected = false>
class Graph {
    std::vector<std::list<std::pair<std::size_t, T>>> adj;
    std::unordered_map<std::size_t, typename std::list<std::pair<std::size_t, T>>::iterator> edges;

    struct PathInfo {
        T d = std::numeric_limits<T>::max();
        std::size_t parent = -1;
    };

public:
    Graph() : adj(n), pinfos(n) {
    }

    std::vector<PathInfo> pinfos;

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

    void InitializeSingleSource(std::size_t s) {
        pinfos.resize(0);
        pinfos.resize(n);
        pinfos[s].d = T {0};
    }

    void Relax(std::size_t u, std::size_t v) {
        T w = getWeight(u, v);
        if (pinfos[v].d > pinfos[u].d + w) {
            pinfos[v].d = pinfos[u].d + w;
            pinfos[v].parent = u;
        }
    }

    bool BellmanFord(std::size_t s) {
        InitializeSingleSource(s);
        for (std::size_t i = 0; i < n - 1; i++) {
            for (std::size_t u = 0; u < n; u++) {
                for (const auto& [v, w] : adj[u]) {
                    Relax(u, v);
                }
            }
        }
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                if (pinfos[v].d > pinfos[u].d + w) {
                    std::vector<std::size_t> cycle;
                    auto curr = v;
                    do {
                        cycle.push_back(curr);
                        curr = pinfos[curr].parent;
                    } while (curr != v);
                    sr::reverse(cycle);

                    std::cout << "Negative cycle: ";
                    for (auto neg_vertex : cycle) {
                        std::cout << neg_vertex << ' ';
                    }
                    std::cout << '\n';
                    return false;
                }
            }
        }
        for (std::size_t u = 0; u < n; u++) {
            printPath(s, u);
            std::cout << '\n';
        }
        return true;
    }

    void printPath(std::size_t source, std::size_t dest) {
        if (dest == source) {
            std::cout << source << " ";
        } else if (pinfos[dest].parent == -1) {
            std::cout << "No path from " << source << " to " << dest << " exists\n";
        } else {
            printPath(source, pinfos[dest].parent);
            std::cout << dest << " ";
        }
    }
};

std::mt19937 gen(std::random_device{}());
std::uniform_real_distribution<> dist(0.01, 10.0);

constexpr std::size_t num_currencies = 20;

Graph<num_currencies + 1> getArbitrageGraph() {
    Graph<num_currencies + 1> g;
    for (std::size_t i = 0; i < num_currencies; i++) {
        for (std::size_t j = i + 1; j < num_currencies; j++) {
            auto rate = dist(gen);
            auto weight = -std::log(rate);
            std::cout << "Currency rate from " << i << " to " << j << " : " << rate << '\n';
            g.addEdge(i, j, weight);
            g.addEdge(j, i, -weight);
        }
        g.addEdge(num_currencies, i, 0);
    }
    return g;
}


int main() {
    auto g = getArbitrageGraph();
    g.BellmanFord(num_currencies);

}