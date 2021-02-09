#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <set>
#include <tuple>
#include <queue>
#include <array>
#include <ranges>
#include <limits>
#include <iterator>

namespace sr = std::ranges;

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

    std::vector<Edge> MSTKruskal() {
        std::vector<Edge> A;
        auto edge_comp = [](const auto& e1, const auto& e2) {
            return std::get<2>(e1) < std::get<2>(e2);
        };
        std::multiset<Edge, decltype(edge_comp)> sorted_edges(edge_comp);
        for (const auto& [idx, it] : edges) {
            std::size_t src = idx / n;
            std::size_t dst = idx % n;
            if (src > dst) {
                std::swap(src, dst); // ensure src < dst, no duplicate in set.
            }
            double w = it->second;
            sorted_edges.emplace(src, dst, w);
        }
        for (const auto& [src, dst, w] : sorted_edges) {
            if (FindSet(src) != FindSet(dst)) {
                A.emplace_back(src, dst, w);
                Union(src, dst);
            }
        }
        return A;
    }

    std::vector<Edge> SecondBestMST(std::vector<Edge>& MST) {
        std::vector<Edge> orig_edges;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                if (u > v) {
                    orig_edges.emplace_back(v, u, w);
                } else {
                    orig_edges.emplace_back(u, v, w);
                }
            }
        }

        Graph g2;
        for (const auto& [src, dst, w] : MST) {
            g2.addEdge(src, dst, w);
        }

        std::array<Edge, n * n> max_table;
        Edge NIL = {0, 0, 0.0};
        for (std::size_t u = 0; u < n; u++) {
            for (std::size_t v = 0; v < n; v++) {
                max_table[u * n + v] = NIL;
            }
            std::queue<std::size_t> Q;
            Q.push(u);
            while (!Q.empty()) {
                auto x = Q.front();
                Q.pop();
                for (const auto& [v, w] : g2.adj[x]) {
                    if (max_table[u * n + v] == NIL && v != u) {
                        if (x == u || w > std::get<2>(max_table[u * n + x])) {
                            max_table[u * n + v] = {x, v, w};
                        } else {
                            max_table[u * n + v] = max_table[u * n + x];
                        }
                        Q.push(v);
                    }
                }
            }
        }

        std::vector<Edge> diffs;
        sr::sort(orig_edges);
        sr::sort(MST);
        sr::set_difference(orig_edges, MST, std::back_inserter(diffs));
        std::size_t diffw = std::numeric_limits<std::size_t>::max();
        Edge argmin = NIL;
        for (const auto& [u, v, w] : diffs) {
            auto diff = std::get<2>(max_table[u * n + v]) - w;
            if (diffw > diff) {
                diffw = diff;
                argmin = {u, v, w};
            }
        }

        auto& [u, v, w] = argmin;
        std::vector<Edge> max_edge = {max_table[u * n + v]};
        std::vector<Edge> secondMST;
        sr::set_difference(MST, max_edge, std::back_inserter(secondMST));
        secondMST.push_back(argmin);

        return secondMST;
    }
};

int main() {
    Graph<9> g1;

    g1.addEdge(0, 1, 4);
    g1.addEdge(0, 7, 8);
    g1.addEdge(1, 2, 8);
    g1.addEdge(1, 7, 11);
    g1.addEdge(2, 3, 7);
    g1.addEdge(2, 5, 4);
    g1.addEdge(2, 8, 2);
    g1.addEdge(3, 4, 9);
    g1.addEdge(3, 5, 14);
    g1.addEdge(4, 5, 10);
    g1.addEdge(5, 6, 2);
    g1.addEdge(6, 7, 1);
    g1.addEdge(6, 8, 6);
    g1.addEdge(7, 8, 7);

    auto g1MST = g1.MSTKruskal();
    std::cout << "MST for g1:\n";
    for (const auto& [src, dst, w] : g1MST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';

    auto g1MST2 = g1.SecondBestMST(g1MST);
    std::cout << "Second MST for g1:\n";
    for (const auto& [src, dst, w] : g1MST2) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';
}
