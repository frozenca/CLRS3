#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <list>
#include <ranges>
#include <stack>
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
    }

    enum class Color {
        White,
        Gray,
        Black,
    };

    bool DFSVisit(std::size_t u, std::vector<Color>& visited, std::vector<std::size_t>& topSort) {
        if (visited[u] == Color::Black) {
            return false;
        } else if (visited[u] == Color::Gray) {
            return true;
        }
        visited[u] = Color::Gray;
        for (auto v : adj[u]) {
            auto res = DFSVisit(v, visited, topSort);
            if (res) {
                return true;
            }
        }
        visited[u] = Color::Black;
        topSort.push_back(u);
        return false;
    }

    std::vector<std::size_t> TopologicalSort() {
        std::stack<std::size_t> S;
        std::vector<std::size_t> topSort;
        std::vector<Color> visited (n, Color::White);

        for (std::size_t u = 0; u < n; u++) {
            if (visited[u] == Color::White) {
                auto res = DFSVisit(u, visited, topSort);
                if (res) {
                    return {};
                }
            }
        }
        sr::reverse(topSort);
        return topSort;
    }

    std::vector<std::size_t> HamiltonianPathDAG() {
        auto topsort = TopologicalSort();
        if (topsort.empty()) {
            return {};
        }
        std::vector<std::size_t> hampath;
        hampath.push_back(topsort[0]);
        for (std::size_t i = 0; i < topsort.size() - 1; i++) {
            if (hasEdge(topsort[i], topsort[i + 1])) {
                hampath.push_back(topsort[i + 1]);
            } else {
                return {};
            }
        }
        return hampath;
    }
};

int main() {
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 4);
    g.addEdge(0, 5);
    g.addEdge(1, 5);
    g.addEdge(2, 5);
    g.addEdge(3, 5);
    g.addEdge(4, 5);

    auto hampath = g.HamiltonianPathDAG();

    for (auto i : hampath) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

}