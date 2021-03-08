#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <stack>
#include <list>

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

    [[nodiscard]] std::unordered_map<std::size_t, std::size_t> getStronglyConnectedComponents() {
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

        std::unordered_map<std::size_t, std::size_t> SCCMap;

        std::vector<int> trans_visited(n);
        std::size_t index = 0;
        for (auto u : toVisit) {
            if (!trans_visited[u]) {
                index++;
                S.push(u);
                SCCMap.emplace(u, index);
                while (!S.empty()) {
                    auto v = S.top();
                    trans_visited[v] = true;
                    bool finished = true;
                    for (auto w : transposed.adj[v]) {
                        if (!trans_visited[w]) {
                            S.push(w);
                            SCCMap.emplace(w, index);
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                        trans_visited[v] = true;
                    }
                }
            }
        }
        return SCCMap;
    }
};

std::size_t getMaxIndex(const std::vector<std::pair<int, int>>& twoSAT) {
    int n = 0;
    for (const auto& [i, j] : twoSAT) {
        n = std::max({n, std::abs(i), std::abs(j)});
    }
    return n;
}

bool isSatisfiable(const std::vector<std::pair<int, int>>& twoSAT) {
    if (twoSAT.empty()) {
        return true;
    }
    auto n = getMaxIndex(twoSAT);
    Graph g(2 * n);
    for (const auto& [i_, j_] : twoSAT) {
        std::size_t i = ((i_ > 0) ? i_ : -i_ + n) - 1;
        std::size_t j = ((j_ > 0) ? j_ : -j_ + n) - 1;

        g.addEdge((i + n) % (2 * n), j);
        g.addEdge((j + n) % (2 * n), i);
    }
    auto SCCMap = g.getStronglyConnectedComponents();
    for (std::size_t i = 0; i < n; i++) {
        if (SCCMap[i] == SCCMap[i + n]) {
            return false;
        }
    }
    return true;
}

int main() {
    std::vector<std::pair<int, int>> twoSAT1 {{1, 2}, {-2, 3}, {4, -1}};
    assert(isSatisfiable(twoSAT1));
    std::vector<std::pair<int, int>> twoSAT2 {{1, 2}, {-2, 1}};
    assert(isSatisfiable(twoSAT2));
    std::vector<std::pair<int, int>> twoSAT3 {{1, -1}};
    assert(!isSatisfiable(twoSAT3));

}
