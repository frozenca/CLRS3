#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <tuple>
#include <algorithm>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace sr = std::ranges;

template <std::size_t n>
class Graph {
    std::vector<std::list<std::pair<std::size_t, std::size_t>>> adj;
    std::unordered_map<std::size_t, std::list<std::pair<std::size_t, std::size_t>>::iterator> edges;

public:
    Graph() : adj(n) {
    }

    void addEdge(std::size_t src, std::size_t dst, std::size_t weight) {
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

    using Edge = std::tuple<std::size_t, std::size_t, std::size_t>;

    std::vector<Edge> MSTPrim(std::size_t r) {
        std::vector<Edge> A;
        std::size_t W = 14;

        std::vector<std::list<std::size_t>> Q (W + 2);
        std::unordered_map<std::size_t, std::pair<std::list<std::size_t>::iterator, std::size_t>> Qmap;
        for (std::size_t i = 0; i < n; i++) {
            if (i == r) {
                Q[0].emplace_front(i);
                Qmap[i] = {Q[0].begin(), 0};
            } else {
                Q[W + 1].emplace_front(i);
                Qmap[i] = {Q[W + 1].begin(), W + 1};
            }
        }

        std::vector<std::pair<std::size_t, std::size_t>> infos (n, {-1, std::numeric_limits<std::size_t>::max()});
        while (sr::any_of(Q, [](const auto& l){return !l.empty();})) {
            auto up = sr::find_if(Q, [](const auto& l){return !l.empty();});
            std::size_t u = up->front();
            up->pop_front();
            Qmap.erase(u);
            for (const auto& [v, w] : adj[u]) {
                if (Qmap.contains(v)) {
                    auto [vp, k] = Qmap[v];
                    if (w < k) {
                        infos[v] = {u, w};
                        Q[k].erase(vp);
                        Q[w].emplace_front(v);
                        Qmap[v] = {Q[w].begin(), w};
                    }
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
