#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>

template <std::size_t n>
class Graph {
    enum class Color {
        White,
        Gray,
        Black
    };
    struct DFSInfo {
        Color color = Color::White;
        std::size_t d = 0;
        std::size_t f = 0;
        std::size_t parent = -1;
    };

    std::vector<std::list<std::size_t>> adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> edges;

public:
    Graph() : adj(n), infos(n) {
    }

    std::vector<DFSInfo> infos;

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].push_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
    }

    void DFSVisit(std::size_t root, std::size_t u, std::size_t& time, std::vector<std::size_t>& minD) {
        time++;
        infos[u].d = time;
        infos[u].color = Color::Gray;
        minD[u] = root;
        for (auto v : adj[u]) {
            if (infos[v].color == Color::White) {
                infos[v].parent = u;
                DFSVisit(root, v, time, minD);
            }
        }
        infos[u].color = Color::Black;
        time++;
        infos[u].f = time;
    }

    void DFS(std::vector<std::size_t>& minD) {
        infos = std::vector<DFSInfo>(n);
        std::size_t time = 0;
        for (std::size_t i = 0; i < n; i++) {
            if (infos[i].color == Color::White) {
                DFSVisit(i, i, time, minD);
            }
        }
    }

    [[nodiscard]] std::vector<std::size_t> computeMinD() {
        Graph transposed;
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : adj[u]) {
                transposed.addEdge(v, u);
            }
        }
        std::vector<std::size_t> minD (n);
        transposed.DFS(minD);
        return minD;
    }


};


int main() {
    Graph<4> g;
    g.addEdge(0, 1);
    g.addEdge(1, 3);
    g.addEdge(2, 0);
    g.addEdge(2, 1);
    g.addEdge(3, 2);
    auto minD = g.computeMinD();
    for (auto n : minD) {
        std::cout << n << ' ';
    }
}
