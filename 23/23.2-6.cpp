#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <cmath>
#include <functional>

template <typename T, typename Comp = std::greater<T>>
void insertionSort(std::vector<T>& A, Comp comp = Comp()) {
    for (size_t j = 1; j < A.size(); j++) {
        T key = A[j];
        size_t i = j - 1;
        while (i < A.size() && comp(A[i], key)) {
            A[i + 1] = A[i];
            i--;
        }
        A[i + 1] = key;
    }
}

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

    void bucketSort(std::vector<Edge>& A) {
        std::vector<std::vector<Edge>> B (A.size());
        for (const auto& [src, dst, d] : A) {
            B[std::floor(A.size() * d)].emplace_back(src, dst, d);
        }
        auto edge_comp = [](const auto& e1, const auto& e2) {return std::get<2>(e1) < std::get<2>(e2);};

        for (auto& v : B) {
            insertionSort(v, edge_comp);
        }
        A.clear();
        for (auto& v : B) {
            A.insert(A.end(), std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
        }
    }

    std::vector<Edge> MSTKruskal() {
        std::vector<Edge> A;
        std::vector<Edge> edges_;
        for (const auto& [idx, it] : edges) {
            std::size_t src = idx / n;
            std::size_t dst = idx % n;
            if (src > dst) {
                std::swap(src, dst); // ensure src < dst, no duplicate in set.
            }
            double w = it->second;
            edges_.emplace_back(src, dst, w);
        }
        
        bucketSort(edges_);

        for (const auto& [src, dst, w] : edges_) {
            if (FindSet(src) != FindSet(dst)) {
                A.emplace_back(src, dst, w);
                Union(src, dst);
            }
        }
        return A;
    }
};

int main() {
    Graph<9> g;
    g.addEdge(0, 1, 4 / 15.0);
    g.addEdge(0, 7, 8 / 15.0);
    g.addEdge(1, 2, 8 / 15.0);
    g.addEdge(1, 7, 11 / 15.0);
    g.addEdge(2, 3, 7 / 15.0);
    g.addEdge(2, 5, 4 / 15.0);
    g.addEdge(2, 8, 2 / 15.0);
    g.addEdge(3, 4, 9 / 15.0);
    g.addEdge(3, 5, 14 / 15.0);
    g.addEdge(4, 5, 10 / 15.0);
    g.addEdge(5, 6, 2 / 15.0);
    g.addEdge(6, 7, 1 / 15.0);
    g.addEdge(6, 8, 6 / 15.0);
    g.addEdge(7, 8, 7 / 15.0);
    auto gMST = g.MSTKruskal();
    for (const auto& [src, dst, w] : gMST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
}
