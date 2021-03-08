#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <tuple>
#include <queue>
#include <unordered_map>

enum class Color {
    White,
    Red,
    Blue,
};

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

    std::vector<Color> BFS(std::size_t source) {
        assert(source < n);
        std::vector<Color> colors (n, Color::White);
        colors[source] = Color::Blue;
        std::queue<std::size_t> q;
        q.push(source);
        while (!q.empty()) {
            auto u = q.front();
            q.pop();
            Color u_color = colors[u];
            for (auto v : adj[u]) {
                if (colors[v] == Color::White) {
                    colors[v] = (u_color == Color::Red) ? Color::Blue : Color::Red;
                    q.push(v);
                } else if (u_color == colors[v]) {
                    return {};
                }
            }
        }
        return colors;
    }
};

int main() {
    {
        Graph g(6);
        g.addEdge(0, 1);
        g.addEdge(1, 2);
        g.addEdge(2, 3);
        g.addEdge(3, 4);
        g.addEdge(4, 5);
        auto colors = g.BFS(0);
        for (auto color : colors) {
            std::cout << static_cast<int>(color) << ' ';
        }
        std::cout << '\n';
    }

    {
        Graph g(5);
        g.addEdge(0, 1);
        g.addEdge(1, 2);
        g.addEdge(2, 3);
        g.addEdge(3, 4);
        g.addEdge(4, 0);
        auto colors = g.BFS(0);
        for (auto color : colors) {
            std::cout << static_cast<int>(color) << ' ';
        }
        std::cout << '\n';
    }



}
