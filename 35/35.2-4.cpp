#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <ranges>
#include <tuple>
#include <stack>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <std::size_t n>
class Graph {
    struct UnionInfo {
        std::size_t index;
        std::size_t parent;
        std::size_t rank = 0;
        UnionInfo(std::size_t index) : index {index}, parent {index} {}
    };

    std::vector<std::list<std::pair<std::size_t, double>>> adj;
    std::unordered_map<std::size_t, std::list<std::pair<std::size_t, double>>::iterator> edges;

    void Link(std::size_t x, std::size_t y) {
        auto& ux = uinfos[x];
        auto& uy = uinfos[y];
        if (ux.rank < uy.rank) {
            uy.parent = x;
        } else {
            ux.parent = y;
            if (ux.rank == uy.rank) {
                uy.rank++;
            }
        }
    }

    std::size_t FindSet(std::size_t x) {
        auto& ux = uinfos[x];
        if (ux.index != ux.parent) {
            ux.parent = FindSet(ux.parent);
        }
        return ux.parent;
    }

    void Union(std::size_t x, std::size_t y) {
        Link(FindSet(x), FindSet(y));
    }

public:
    Graph() : adj(n) {
        for (std::size_t i = 0; i < n; i++) {
            uinfos.emplace_back(i);
        }
    }

    std::vector<UnionInfo> uinfos;

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

    std::pair<std::vector<Edge>, std::vector<Edge>> divideEdgesByMedian(std::vector<Edge>& E) {
        auto edge_comp = [](const auto& e1, const auto& e2) {return std::get<2>(e1) > std::get<2>(e2);};

        auto half = E.size() / 2;
        sr::nth_element(E, E.begin() + half, edge_comp);
        std::vector<Edge> A (E.begin(), E.begin() + half);
        std::vector<Edge> B (E.begin() + half, E.end());
        return {A, B};
    }

    std::pair<bool, std::vector<Edge>> findForests() {
        bool is_connected = true;
        std::vector<Edge> F;
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
                            F.emplace_back(v, next, w);
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

        return {is_connected, F};
    }

    std::vector<Edge> MBST(std::vector<Edge>& E) {
        if (E.size() <= 1) {
            return E;
        }
        auto [A, B] = divideEdgesByMedian(E);
        Graph G_B;
        for (const auto& [u, v, w] : B) {
            G_B.addEdge(u, v, w);
        }
        auto [is_connected, F] = G_B.findForests();
        if (is_connected) {
            return G_B.MBST();
        } else {
            Graph G_A;
            for (const auto& [u, v, w] : A) {
                G_A.addEdge(u, v, w);
            }
            auto G_A_MBST = G_A.MBST();
            sr::copy(F, std::back_inserter(G_A_MBST));
            return G_A_MBST;
        }
        return E;
    }

    std::vector<Edge> MBST() {
        std::vector<Edge> E = getEdges();
        return MBST(E);
    }

    void DFSVisit(std::size_t u, std::vector<int>& visited, const std::vector<std::vector<std::size_t>>& tree,
                  std::list<std::size_t>& walk) {
        visited[u] = true;
        walk.push_back(u);
        for (auto v : tree[u]) {
            if (!visited[v]) {
                DFSVisit(v, visited, tree, walk);
                walk.push_back(u);
            }
        }
    }

    std::vector<std::size_t> getMBSTWalk() {
        auto MBST_raw = MBST();
        std::vector<std::vector<std::size_t>> tree (n);
        for (const auto& [src, dst, w] : MBST_raw) {
            tree[src].push_back(dst);
            tree[dst].push_back(src);
        }
        std::list<std::size_t> walk;
        std::vector<int> visited (n);
        for (std::size_t i = 0; i < n; i++) {
            if (!visited[i]) {
                DFSVisit(i, visited, tree, walk);
            }
        }
        std::vector<int> seen (n);
        std::vector<std::size_t> skipped_walk;
        for (auto w : walk) {
            if (!seen[w]) {
                skipped_walk.push_back(w);
                seen[w] = true;
            }
        }

        return skipped_walk;
    }
};

int main() {
    Graph<7> g;
    g.addEdge(0, 1, 7);
    g.addEdge(0, 3, 5);
    g.addEdge(1, 2, 8);
    g.addEdge(1, 3, 9);
    g.addEdge(1, 4, 7);
    g.addEdge(2, 4, 5);
    g.addEdge(3, 4, 15);
    g.addEdge(3, 5, 6);
    g.addEdge(4, 5, 8);
    g.addEdge(4, 6, 7);
    g.addEdge(5, 6, 11);

    auto MBSTWalk = g.getMBSTWalk();
    for (auto idx : MBSTWalk) {
        std::cout << idx << ' ';
    }
    std::cout << '\n';
}
