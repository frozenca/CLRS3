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

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (edges.contains(src * n + dst)) {
            adj[src].erase(edges[src * n + dst]);
            edges.erase(src * n + dst);
        }
        if (edges.contains(dst * n + src)) {
            adj[dst].erase(edges[dst * n + src]);
            edges.erase(dst * n + src);
        }
    }

    [[nodiscard]] std::unordered_set<std::size_t> VertexCoverTree() {
        std::unordered_set<std::size_t> L;
        for (std::size_t i = 0; i < n; i++) {
            if (adj[i].size() == 1) {
                L.insert(i);
            }
        }
        if (L.size() <= 1) {
            return L;
        }
        std::unordered_set<std::size_t> C;
        while (!L.empty()) {
            auto v = *L.begin();
            auto u = adj[v].front();
            C.insert(u);
            L.erase(v);
            auto adj_temp = adj[u];
            for (auto w : adj_temp) {
                removeEdge(u, w);
                if (adj[w].size() == 1) {
                    L.insert(w);
                } else if (adj[w].empty()) {
                    L.erase(w);
                }
            }
        }
        return C;
    }

};

int main() {
    Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(2, 6);

    auto VC = g.VertexCoverTree();

    for (auto v : VC) {
        std::cout << v << ' ';
    }
    std::cout << '\n';

}