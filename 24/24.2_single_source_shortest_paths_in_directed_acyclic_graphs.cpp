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

    void DFSVisit(std::size_t u, std::vector<int>& visited, std::list<std::size_t>& topSort) {
        visited[u] = true;
        for (const auto& [v, w] : adj[u]) {
            if (!visited[v]) {
                DFSVisit(v, visited, topSort);
            }
        }
        topSort.push_front(u);
    }

    std::list<std::size_t> TopologicalSort() {
        std::stack<std::size_t> S;
        std::list<std::size_t> topSort;
        std::vector<int> visited (n);

        for (std::size_t u = 0; u < n; u++) {
            if (!visited[u]) {
                DFSVisit(u, visited, topSort);
            }
        }
        return topSort;
    }

    void DAGShortestPaths(std::size_t s) {
        auto TS = TopologicalSort();
        InitializeSingleSource(s);
        for (auto u : TS) {
            for (const auto& [v, w] : adj[u]) {
                Relax(u, v);
            }
        }

        for (std::size_t u = 0; u < n; u++) {
            printPath(s, u);
            std::cout << '\n';
        }
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

int main() {
    Graph<6> g;
    g.addEdge(0, 1, 5);
    g.addEdge(0, 2, 3);
    g.addEdge(1, 2, 2);
    g.addEdge(1, 3, 6);
    g.addEdge(2, 3, 7);
    g.addEdge(2, 4, 4);
    g.addEdge(2, 5, 2);
    g.addEdge(3, 4, -1);
    g.addEdge(3, 5, 1);
    g.addEdge(4, 5, -2);

    g.DAGShortestPaths(0);

}