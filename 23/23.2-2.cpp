#include <cassert>
#include <iostream>
#include <utility>
#include <array>
#include <vector>
#include <algorithm>
#include <ranges>

namespace sr = std::ranges;

template <std::size_t n>
class Graph {
    std::array<double, n * n> adj;
public:
    Graph() {
        sr::fill(adj, 0.0);
    }

    void addEdge(std::size_t src, std::size_t dst, double weight) {
        assert(src < n && dst < n);
        adj[src * n + dst] = weight;
        adj[dst * n + src] = weight;
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src * n + dst] = 0;
        adj[dst * n + src] = 0;
    }

    using Edge = std::tuple<std::size_t, std::size_t, double>;

    std::vector<Edge> MSTPrim(std::size_t r) {
        std::vector<Edge> A;
        std::vector<std::pair<std::size_t, double>> infos (n, {-1, std::numeric_limits<double>::max()});
        std::vector<int> visited (n);
        auto info_comp = [](const auto& i1, const auto& i2) {
            return i1.second < i2.second;
        };
        infos[r] = {-1, 0.0};

        while (sr::count(visited, 0)) {
            std::size_t u = -1;
            double d = std::numeric_limits<double>::max();
            for (std::size_t k = 0; k < n; k++) {
                if (!visited[k] && infos[k].second < d) {
                    d = infos[k].second;
                    u = k;
                }
            }
            visited[u] = 1;
            for (std::size_t v = 0; v < n; v++) {
                if (!visited[v] && adj[v * n + u] != 0.0 && adj[v * n + u] < infos[v].second) {
                    infos[v] = {u, adj[v * n + u]};
                }
            }
        }

        for (std::size_t i = 0; i < n; i++) {
            if (i != r) {
                A.emplace_back(i, infos[i].first, infos[i].second);
            }
        }
        return A;
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
    auto gMST = g.MSTPrim(0);
    for (const auto& [src, dst, w] : gMST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
}
