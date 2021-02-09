#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <set>

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
};

int main() {
    Graph<9> g1;
    Graph<10> g2, g3;

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

    g2.addEdge(0, 1, 4);
    g2.addEdge(0, 7, 8);
    g2.addEdge(1, 2, 8);
    g2.addEdge(1, 7, 11);
    g2.addEdge(2, 3, 7);
    g2.addEdge(2, 5, 4);
    g2.addEdge(2, 8, 2);
    g2.addEdge(3, 4, 9);
    g2.addEdge(3, 5, 14);
    g2.addEdge(4, 5, 10);
    g2.addEdge(5, 6, 2);
    g2.addEdge(6, 7, 1);
    g2.addEdge(6, 8, 6);
    g2.addEdge(7, 8, 7);

    g2.addEdge(4, 9, 6);
    g2.addEdge(3, 9, 8);
    g2.addEdge(1, 9, 3);
    g2.addEdge(2, 9, 5);
    g2.addEdge(6, 9, 4);

    g3.addEdge(4, 9, 6);
    g3.addEdge(3, 9, 8);
    g3.addEdge(1, 9, 3);
    g3.addEdge(2, 9, 5);
    g3.addEdge(6, 9, 4);

    auto g1MST = g1.MSTKruskal();
    std::cout << "MST for g1:\n";
    for (const auto& [src, dst, w] : g1MST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
        g3.addEdge(src, dst, w);
    }
    std::cout << '\n';

    auto g2MST = g2.MSTKruskal();
    std::cout << "MST for g2:\n";
    for (const auto& [src, dst, w] : g2MST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';

    auto g3MST = g3.MSTKruskal();
    std::cout << "MST for g3:\n";
    for (const auto& [src, dst, w] : g3MST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';
}
