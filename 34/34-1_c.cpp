#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <ranges>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <tuple>
#include <stack>
#include <list>

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

    [[nodiscard]] std::vector<std::unordered_set<std::size_t>> getStronglyConnectedComponents() {
        std::vector<int> visited(n);
        std::stack<std::size_t> S;
        std::list<std::size_t> toVisit;
        for (std::size_t u = 0; u < n; u++) {
            if (!visited[u]) {
                S.push(u);
                while (!S.empty()) {
                    auto v = S.top();
                    visited[v] = true;
                    bool finished = true;
                    for (auto w : adj[v]) {
                        if (!visited[w]) {
                            S.push(w);
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                        visited[v] = true;
                        toVisit.push_front(v);
                    }
                }
            }
        }

        Graph transposed (n);

        for (std::size_t i = 0; i < n; i++) {
            for (auto neighbor : adj[i]) {
                transposed.addEdge(neighbor, i);
            }
        }

        std::vector<std::unordered_set<std::size_t>> SCCs;

        std::vector<int> trans_visited(n);
        for (auto u : toVisit) {
            if (!trans_visited[u]) {
                std::unordered_set<std::size_t> SCC;
                S.push(u);
                SCC.insert(u);
                while (!S.empty()) {
                    auto v = S.top();
                    trans_visited[v] = true;
                    bool finished = true;
                    for (auto w : transposed.adj[v]) {
                        if (!trans_visited[w]) {
                            S.push(w);
                            SCC.insert(w);
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                        trans_visited[v] = true;
                    }
                }
                SCCs.push_back(std::move(SCC));
            }
        }
        return SCCs;
    }

    [[nodiscard]] std::size_t SizeMaximalIndependentSetDegreeTwo() {
        assert(sr::all_of(adj, [](const auto& l){return l.size() == 2;}));
        auto SCCs = getStronglyConnectedComponents();
        std::size_t sz = 0;
        for (const auto& SCC : SCCs) {
            sz = std::max(sz, SCC.size() / 2);
        }
        return sz;
    }
};

int main() {
    Graph g(9);
    g.addEdge(0, 1);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 2);
    g.addEdge(5, 6);
    g.addEdge(6, 7);
    g.addEdge(7, 8);
    g.addEdge(8, 6);
    std::cout << g.SizeMaximalIndependentSetDegreeTwo() << '\n';

}
