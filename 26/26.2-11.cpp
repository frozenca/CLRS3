#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

template <std::size_t n>
class FlowNetwork final {

    std::size_t source = 0;
    std::size_t sink = n - 1;

    // (dst, capacity, flow)
    using Edge = std::tuple<std::size_t, std::size_t, std::size_t>;

    std::vector<std::list<Edge>> adj;
    std::unordered_map<std::size_t, std::list<Edge>::iterator> edges;

    // (dst, res_capacity)
    using ResidualEdge = std::pair<std::size_t, std::size_t>;
    std::vector<std::list<ResidualEdge>> res;
    std::unordered_map<std::size_t, std::list<ResidualEdge>::iterator> res_edges;

public:
    FlowNetwork(std::size_t source = 0, std::size_t sink = n - 1) : adj(n), res(n),
    source(source), sink(sink) {
        assert(source != sink && source < n && sink < n);
    }

    FlowNetwork(const FlowNetwork& g) : adj(n) {
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c] : g.adj[u]) {
                addEdge(u, v, c);
            }
        }
    }

    FlowNetwork& operator=(const FlowNetwork& g) {
        if (&g == this) {
            return *this;
        }
        adj.clear();
        adj.resize(n);
        edges.clear();

        res.clear();
        res.resize(n);
        res_edges.clear();

        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c] : g.adj[u]) {
                addEdge(u, v, c);
            }
        }
    }

    FlowNetwork(FlowNetwork&& g) noexcept : adj(std::move(g.adj)), edges(std::move(g.edges)),
    res(std::move(g.res)), res_edges(std::move(g.res_edges))
    {}

    FlowNetwork& operator=(FlowNetwork&& g) noexcept {
        adj = std::move(g.adj);
        edges = std::move(g.edges);
        res = std::move(g.res);
        res_edges = std::move(g.res_edges);
        return *this;
    }

    void addEdge(std::size_t src, std::size_t dst, std::size_t capacity) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, capacity, 0);
            edges[src * n + dst] = adj[src].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        if (res_edges.contains(src * n + dst)) {
            res[src].erase(res_edges[src * n + dst]);
            res_edges.erase(src * n + dst);
        }
        if (res_edges.contains(dst * n + src)) {
            res[dst].erase(res_edges[dst * n + src]);
            res_edges.erase(dst * n + src);
        }
    }

    [[nodiscard]] std::size_t getCapacity(std::size_t src, std::size_t dst) const {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return 0;
        } else {
            return std::get<1>(*edges.at(src * n + dst));
        }
    }

    [[nodiscard]] std::size_t getFlow(std::size_t src, std::size_t dst) const {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return 0;
        } else {
            return std::get<2>(*edges.at(src * n + dst));
        }
    }

    [[nodiscard]] std::size_t getResidual(std::size_t src, std::size_t dst) const {
        assert(src < n && dst < n);
        if (!res_edges.contains(src * n + dst)) {
            return 0;
        } else {
            return res_edges.at(src * n + dst)->second;
        }
    }

    void setFlow(std::size_t src, std::size_t dst, std::size_t new_flow) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return;
        } else {
            auto& [_, capacity, flow] = *edges.at(src * n + dst);
            flow = new_flow;
            if (capacity > flow) {
                if (res_edges.contains(src * n + dst)) {
                    res_edges[src * n + dst]->second = capacity - flow;
                } else {
                    res[src].emplace_front(dst, capacity - flow);
                    res_edges[src * n + dst] = res[src].begin();
                }
                if (flow) {
                    if (res_edges.contains(dst * n + src)) {
                        res_edges[dst * n + src]->second = flow;
                    } else {
                        res[dst].emplace_front(src, flow);
                        res_edges[dst * n + src] = res[dst].begin();
                    }
                }
            } else if (capacity == flow) {
                if (res_edges.contains(src * n + dst)) {
                    res[src].erase(res_edges[src * n + dst]);
                    res_edges.erase(src * n + dst);
                }
                if (res_edges.contains(dst * n + src)) {
                    res[dst].erase(res_edges[dst * n + src]);
                    res_edges.erase(dst * n + src);
                }
            } else {
                if (res_edges.contains(src * n + dst)) {
                    res[src].erase(res_edges[src * n + dst]);
                    res_edges.erase(src * n + dst);
                }
                if (res_edges.contains(dst * n + src)) {
                    res_edges[dst * n + src]->second = flow;
                } else {
                    res[dst].emplace_front(src, flow);
                    res_edges[dst * n + src] = res[dst].begin();
                }
            }
        }
    }

    std::pair<bool, std::vector<std::size_t>> getAugmentingPath() {
        std::vector<std::size_t> prev (n, -1);
        std::queue<std::size_t> q;
        q.push(source);
        while (!q.empty()) {
            auto curr = q.front();
            q.pop();
            for (const auto& [next, f] : res[curr]) {
                if (prev[next] == -1) {
                    prev[next] = curr;
                    q.push(next);
                }
            }
        }
        return {prev[sink] != -1, prev};
    }

    std::size_t EdmondsKarp() {
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c, f] : adj[u]) {
                setFlow(u, v, 0);
            }
        }
        std::size_t maximum_flow = 0;
        while (true) {
            auto [augmentable, argPathPrev] = getAugmentingPath();
            if (!augmentable) {
                break;
            }
            std::size_t res_capacity = std::numeric_limits<std::size_t>::max();
            for (std::size_t u = argPathPrev[sink], v = sink;
                    u != -1 && v != source;
                    v = u, u = argPathPrev[u]) {
                res_capacity = std::min(res_capacity, getResidual(u, v));
            }

            for (std::size_t u = argPathPrev[sink], v = sink;
                 u != -1 && v != source;
                 v = u, u = argPathPrev[u]) {
                if (edges.contains(u * n + v)) {
                    setFlow(u, v, getFlow(u, v) + res_capacity);
                } else {
                    assert(edges.contains(v * n + u));
                    setFlow(v, u, getFlow(v, u) - res_capacity);
                }
            }
            maximum_flow += res_capacity;
        }
        return maximum_flow;
    }
};

template <std::size_t n, bool is_undirected = false>
class Graph final {
    std::vector<std::list<std::size_t>> adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> edges;

public:
    Graph() : adj(n) {
    }

    Graph(const Graph& g) : adj(n) {
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : g.adj) {
                addEdge(u, v);
            }
        }
    }

    Graph& operator=(const Graph& g) {
        if (&g == this) {
            return *this;
        }
        adj.clear();
        adj.resize(n);
        edges.clear();
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : g.adj) {
                addEdge(u, v);
            }
        }
    }

    Graph(Graph&& g) noexcept : adj(std::move(g.adj)), edges(std::move(g.edges)) {}

    Graph& operator=(Graph&& g) noexcept {
        adj = std::move(g.adj);
        edges = std::move(g.edges);
        return *this;
    }

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
        if (is_undirected) {
            if (!edges.contains(dst * n + src)) {
                adj[dst].emplace_front(src);
                edges[dst * n + src] = adj[dst].begin();
            }
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        if (is_undirected) {
            adj[dst].erase(edges[dst * n + src]);
            edges.erase(dst * n + src);
        }
    }

    std::size_t getEdgeConnectivity() {
        std::size_t u = 0;
        std::size_t k = std::numeric_limits<std::size_t>::max();
        for (std::size_t v = 1; v < n; v++) {
            FlowNetwork<n> network(0, v);
            for (std::size_t a = 0; a < n; a++) {
                for (auto b : adj[a]) {
                    network.addEdge(a, b, 1);
                    network.addEdge(b, a, 1);
                }
            }
            k = std::min(k, network.EdmondsKarp());
        }
        return k;
    }

};

int main() {
    FlowNetwork<6> network;
    network.addEdge(0, 1, 16);
    network.addEdge(0, 2, 13);
    network.addEdge(1, 3, 12);
    network.addEdge(2, 1, 4);
    network.addEdge(2, 4, 14);
    network.addEdge(3, 2, 9);
    network.addEdge(3, 5, 20);
    network.addEdge(4, 3, 7);
    network.addEdge(4, 5, 4);
    network.EdmondsKarp();

    Graph<5, true> g;
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    std::cout << "Edge connectivity of g " << g.getEdgeConnectivity() << '\n';

    Graph<5, true> g2;
    g2.addEdge(0, 1);
    g2.addEdge(1, 2);
    g2.addEdge(2, 3);
    g2.addEdge(3, 4);
    g2.addEdge(4, 0);
    std::cout << "Edge connectivity of g2 " << g2.getEdgeConnectivity() << '\n';


}