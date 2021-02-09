#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <tuple>
#include <ranges>
#include <set>
#include <stack>

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

    std::vector<Edge> getEdges() {
        std::vector<Edge> E;
        for (const auto& [idx, it] : edges) {
            std::size_t src = idx / n;
            std::size_t dst = idx % n;
            if (src < dst) {
                double w = it->second;
                E.emplace_back(src, dst, w);
            }
        }
        return E;
    }

    bool isConnected() {
        bool is_connected = true;
        std::vector<int> visited (n);
        bool is_first = true;
        for (std::size_t u = 0; u < n; u++) {
            if (!visited[u]) {
                if (!is_first) {
                    is_connected = false;
                }
                std::stack<std::size_t> S;
                S.push(u);
                while (!S.empty()) {
                    auto v = S.top();
                    visited[v] = true;
                    bool finished = true;
                    for (const auto& [next, w] : adj[v]) {
                        if (!visited[next]) {
                            S.push(next);
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                    }
                }
                is_first = false;
            }
        }
        return is_connected;
    }

    std::pair<std::size_t, std::size_t> DFSVisit(std::size_t u, std::vector<int>& visited, std::vector<std::size_t>& parents) {
        visited[u] = 1;
        for (const auto& [v, w] : adj[u]) {
            if (!visited[v]) {
                parents[v] = u;
                return DFSVisit(v, visited, parents);
            } else if (parents[u] != v) {
                return {u, v};
            }
        }
        return {-1, - 1};
    }

    std::vector<Edge> hasCycle() {

        std::vector<Edge> cycle;
        std::vector<std::size_t> parents(n, -1);
        for (std::size_t i = 0; i < n; i++) {
            std::vector<int> visited (n);
            auto[u, v] = DFSVisit(i, visited, parents);
            if (u != -1) {
                cycle.emplace_back(u, v, edges[u * n + v]->second);
                if (parents[u] != -1) {
                    do {
                        auto p = parents[u];
                        cycle.emplace_back(u, p, edges[u * n + p]->second);
                        u = p;
                    } while (u != v && parents[u] != -1);
                }
            }
            if (cycle.size() <= 1 || std::get<1>(cycle.back()) != std::get<1>(cycle.front())) {
                cycle.clear();
            } else {
                return cycle;
            }
        }
        return cycle;
    }

    std::vector<Edge> MaybeMSTA() {
        auto E = getEdges();
        auto edge_comp = [](const auto& e1, const auto& e2) {return std::get<2>(e1) > std::get<2>(e2);};
        sr::sort(E, edge_comp);
        std::vector<Edge> T (E.begin(), E.end());
        for (const auto& e : E) {
            T.erase(sr::find(T, e));
            Graph g;
            for (const auto& [u, v, w] : T) {
                g.addEdge(u, v, w);
            }
            if (!g.isConnected()) {
                T.push_back(e);
            }
        }
        return T;
    }

    std::vector<Edge> MaybeMSTB() {
        auto E = getEdges();
        std::vector<Edge> T;
        for (const auto& e : E) {
            T.push_back(e);
            Graph g;
            for (const auto& [u, v, w] : T) {
                g.addEdge(u, v, w);
            }
            if (!g.hasCycle().empty()) {
                T.pop_back();
            }
        }
        return T;
    }

    std::vector<Edge> MaybeMSTC() {
        auto E = getEdges();
        std::vector<Edge> T;
        auto edge_comp = [](const auto& e1, const auto& e2) {return std::get<2>(e1) < std::get<2>(e2);};
        for (const auto& e : E) {
            T.push_back(e);
            Graph g;
            for (const auto& [u, v, w] : T) {
                g.addEdge(u, v, w);
            }
            auto c = g.hasCycle();
            if (!c.empty()) {
                auto e_ = *sr::max_element(c, edge_comp);
                if (std::get<0>(e_) > std::get<1>(e_)) {
                    e_ = {std::get<1>(e_), std::get<0>(e_), std::get<2>(e_)};
                }
                T.erase(sr::find(T, e_));
            }
        }
        Graph g;
        for (const auto& [u, v, w] : T) {
            g.addEdge(u, v, w);
        }
        auto c = g.hasCycle();
        if (!c.empty()) {
            auto e_ = *sr::max_element(c, edge_comp);
            if (std::get<0>(e_) > std::get<1>(e_)) {
                e_ = {std::get<1>(e_), std::get<0>(e_), std::get<2>(e_)};
            }
            T.erase(sr::find(T, e_));
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

    auto A = g.MaybeMSTA();
    std::cout << "Maybe MST for g - A:\n";
    for (const auto& [src, dst, w] : A) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';

    auto B = g.MaybeMSTB();
    std::cout << "Maybe MST for g - B:\n";
    for (const auto& [src, dst, w] : B) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';

    auto C = g.MaybeMSTC();
    std::cout << "Maybe MST for g - C:\n";
    for (const auto& [src, dst, w] : C) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';
}
