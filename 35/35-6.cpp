#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <tuple>
#include <memory>
#include <algorithm>
#include <limits>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace sr = std::ranges;

template <std::size_t n>
class Graph {
    std::vector<std::list<std::pair<std::size_t, double>>> adj;
    std::unordered_map<std::size_t, std::list<std::pair<std::size_t, double>>::iterator> edges;

public:
    Graph() : adj(n) {
    }

    void addEdge(std::size_t src, std::size_t dst, double weight) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, weight);
            edges[src * n + dst] = adj[src].begin();
        }
        if (!edges.contains(dst * n + src)) {
            adj[dst].emplace_front(src, weight);
            edges[dst * n + src] = adj[dst].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        adj[dst].erase(edges[dst * n + src]);
        edges.erase(dst * n + src);
    }

    using Edge = std::tuple<std::size_t, std::size_t, double>;

    std::vector<Edge> ApproxMaxSpanningTree() {
        std::vector<Edge> T;
        for (std::size_t i = 0; i < n; i++) {
            auto max_w = std::numeric_limits<double>::lowest();
            auto b = i;
            for (const auto& [u, w] : adj[i]) {
                if (w >= max_w) {
                    max_w = w;
                    b = u;
                }
            }
            if (b != i) {
                T.emplace_back(i, b, max_w);
            }
        }
        return T;
    }
};

int main() {
    Graph<9> g;
    g.addEdge(0, 1, 4);
    g.addEdge(0, 7, 8);
    g.addEdge(1, 2, 8);
    g.addEdge(1, 7, 11);
    g.addEdge(2, 3, 7);
    g.addEdge(2, 5, 4);
    g.addEdge(2, 8, 2);
    g.addEdge(3, 4, 9);
    g.addEdge(3, 5, 14);
    g.addEdge(4, 5, 10);
    g.addEdge(5, 6, 2);
    g.addEdge(6, 7, 1);
    g.addEdge(6, 8, 6);
    g.addEdge(7, 8, 7);
    auto AMST = g.ApproxMaxSpanningTree();
    for (const auto& [src, dst, w] : AMST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
}
