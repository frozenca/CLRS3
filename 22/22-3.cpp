#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <forward_list>

template <std::size_t n>
class Graph {
    std::vector<std::list<std::size_t>> adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> edges;

public:
    Graph() : adj(n) {
    }

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].push_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
    }

    using PairList = std::forward_list<std::pair<std::size_t, std::list<std::size_t>::iterator>>;

    [[nodiscard]] std::list<std::size_t> Visit(PairList& L, std::size_t v) {
        std::list<std::size_t> C;
        std::size_t u = v;
        while (!adj[u].empty()) {
            auto w = *adj[u].begin();
            adj[u].erase(adj[u].begin());
            C.push_front(u);
            if (!adj[u].empty()) {
                L.emplace_front(u, C.begin());
            }
            u = w;
        }
        C.reverse();
        return C;
    }

    [[nodiscard]] std::list<std::size_t> getEulerianTour() {
        std::list<std::size_t> T;
        PairList L;
        L.emplace_front(0, T.begin());
        while (!L.empty()) {
            auto [v, location] = L.front();
            L.pop_front();
            auto C = Visit(L, v);
            if (location == T.end()) {
                T = C;
            } else {
                T.splice(location, C);
            }
        }
        return T;
    }

};


int main() {
    Graph<4> g;
    g.addEdge(0, 1);
    g.addEdge(1, 3);
    g.addEdge(2, 0);
    g.addEdge(2, 1);
    g.addEdge(3, 2);
    auto T = g.getEulerianTour();
    for (auto n : T) {
        std::cout << n << ' ';
    }
}
