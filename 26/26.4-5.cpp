#include <algorithm>
#include <cassert>
#include <cstddef>
#include <chrono>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <queue>
#include <random>
#include <ranges>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace crn = std::chrono;
namespace sr = std::ranges;

template <std::size_t n>
class FlowNetwork final {

    std::size_t source = 0;
    std::size_t sink = n - 1;

    // (dst, capacity, flow)
    using Edge = std::tuple<std::size_t, std::size_t, std::size_t>;

    std::vector<std::list<Edge>> adj;
    std::unordered_map<std::size_t, std::list<Edge>::iterator> edges;

    // (dst)
    std::vector<std::list<std::size_t>> undirected_adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> undirected_edges;

    // (dst, res_capacity)
    using ResidualEdge = std::pair<std::size_t, std::size_t>;
    std::vector<std::list<ResidualEdge>> res;
    std::unordered_map<std::size_t, std::list<ResidualEdge>::iterator> res_edges;

public:
    FlowNetwork(std::size_t source = 0, std::size_t sink = n - 1) : adj(n), undirected_adj(n), res(n),
                                                                    source (source), sink (sink) {}

    FlowNetwork(const FlowNetwork& g) : adj(n) {
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c] : g.adj) {
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

        undirected_adj.clear();
        undirected_adj.resize(n);
        undirected_edges.clear();

        res.clear();
        res.resize(n);
        res_edges.clear();

        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c] : g.adj) {
                addEdge(u, v, c);
            }
        }
    }

    FlowNetwork(FlowNetwork&& g) noexcept = default;

    FlowNetwork& operator=(FlowNetwork&& g) noexcept = default;

    void addEdge(std::size_t src, std::size_t dst, std::size_t capacity) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst) && !edges.contains(dst * n + src)) {
            adj[src].emplace_front(dst, capacity, 0);
            edges[src * n + dst] = adj[src].begin();
            undirected_adj[src].emplace_front(dst);
            undirected_edges[src * n + dst] = undirected_adj[src].begin();
            undirected_adj[dst].emplace_front(src);
            undirected_edges[dst * n + src] = undirected_adj[dst].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        undirected_adj[src].erase(undirected_edges[src * n + dst]);
        undirected_edges.erase(src * n + dst);
        undirected_adj[dst].erase(undirected_edges[dst * n + src]);
        undirected_edges.erase(dst * n + src);
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
            } else if (res_edges.contains(src * n + dst)) {
                res[src].erase(res_edges[src * n + dst]);
                res_edges.erase(src * n + dst);
            }
            if (flow) {
                if (res_edges.contains(dst * n + src)) {
                    res_edges[dst * n + src]->second = flow;
                } else {
                    res[dst].emplace_front(src, flow);
                    res_edges[dst * n + src] = res[dst].begin();
                }
            } else if (res_edges.contains(dst * n + src)) {
                res[dst].erase(res_edges[dst * n + src]);
                res_edges.erase(dst * n + src);
            }
        }
    }

    void InitializePreflow(std::vector<int>& excess, std::vector<std::size_t>& height,
                           std::vector<std::list<std::size_t>::iterator>& current_edges) {
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c, f] : adj[u]) {
                setFlow(u, v, 0);
            }
        }
        height[source] = n;
        for (auto& [v, c, f] : adj[source]) {
            setFlow(source, v, c);
            excess[v] = c;
            excess[source] -= static_cast<int>(c);
        }
        for (std::size_t u = 0; u < n; u++) {
            current_edges[u] = undirected_adj[u].begin();
        }
    }

    bool Pushable(std::size_t u, std::size_t v,
                  std::vector<int>& excess, const std::vector<std::size_t>& height) {
        return excess[u] > 0 && static_cast<int>(getResidual(u, v)) > 0 && height[u] == height[v] + 1;
    }

    void Push(std::size_t u, std::size_t v, std::vector<int>& excess) {
        auto delta = std::min(excess[u], static_cast<int>(getResidual(u, v)));
        if (edges.contains(u * n + v)) {
            setFlow(u, v, getFlow(u, v) + delta);
        } else {
            assert(edges.contains(v * n + u));
            setFlow(v, u, getFlow(v, u) - delta);
        }
        excess[u] -= delta;
        excess[v] += delta;
    }

    bool Relabelable(std::size_t u,
                     const std::vector<int>& excess, std::vector<std::size_t>& height) {
        if (excess[u] <= 0) {
            return false;
        }
        for (const auto& [v, _] : res[u]) {
            if (height[u] > height[v]) {
                return false;
            }
        }
        return true;
    }

    void Relabel(std::size_t u, std::vector<std::size_t>& height) {
        std::size_t min_height = std::numeric_limits<std::size_t>::max();
        for (const auto& [v, _] : res[u]) {
            min_height = std::min(min_height, height[v]);
        }
        height[u] = min_height + 1;
    }

    [[nodiscard]] bool isInL(std::size_t u) const {
        return u != source && u != sink && std::get<0>(adj[u].front()) != sink;
    }

    [[nodiscard]] bool isInR(std::size_t u) const {
        return u != source && u != sink && std::get<0>(adj[u].front()) == sink;
    }

    std::size_t PushRelabelBipartite() {
        std::vector<int> excess (n);
        std::vector<std::size_t> height (n);
        std::vector<std::list<std::size_t>::iterator> current_edges(n);
        InitializePreflow(excess, height, current_edges);

        bool active = true;
        while (active) {
            active = false;
            for (std::size_t u = 0; u < n; u++) {
                if (isInL(u)) {
                    if (Relabelable(u, excess, height)) {
                        Relabel(u, height);
                        while (!Pushable(u, *current_edges[u], excess, height)) {
                            std::advance(current_edges[u], 1);
                            if (current_edges[u] == undirected_adj[u].end()) {
                                current_edges[u] = undirected_adj[u].begin();
                            }
                        }
                        Push(u, *current_edges[u], excess);
                        active = true;
                    }

                }
            }
            for (std::size_t u = 0; u < n; u++) {
                if (isInR(u)) {
                    if (Relabelable(u, excess, height)) {
                        Relabel(u, height);
                        while (!Pushable(u, *current_edges[u], excess, height)) {
                            std::advance(current_edges[u], 1);
                            if (current_edges[u] == undirected_adj[u].end()) {
                                current_edges[u] = undirected_adj[u].begin();
                            }
                        }
                        Push(u, *current_edges[u], excess);
                        active = true;
                    }
                }
            }
            for (std::size_t u = 0; u < n; u++) {
                if (isInR(u)) {
                    while (!Pushable(u, *current_edges[u], excess, height)) {
                        std::advance(current_edges[u], 1);
                        if (current_edges[u] == undirected_adj[u].end()) {
                            current_edges[u] = undirected_adj[u].begin();
                            break;
                        }
                    }
                    if (Pushable(u, *current_edges[u], excess, height)) {
                        Push(u, *current_edges[u], excess);
                        active = true;
                    }
                }
            }
        }

        std::size_t max_flow = 0;
        for (const auto& [v, c, f] : adj[source]) {
            max_flow += f;
        }
        return max_flow;
    }
};

template <std::size_t n>
class BipartiteGraph final {
    std::vector<std::list<std::size_t>> adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> edges;
    std::unordered_set<std::size_t> L;
    std::unordered_set<std::size_t> R;

public:
    BipartiteGraph(std::initializer_list<std::size_t> L_) : adj(n), L (L_) {
        assert(sr::all_of(L, [](auto num){return num < n;}) && L.size() < n);
        for (std::size_t u = 0; u < n; u++) {
            if (!L.contains(u)) {
                R.insert(u);
            }
        }
    }

    BipartiteGraph(const BipartiteGraph& g) : adj(n), L(g.L), R(g.R) {
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : g.adj[u]) {
                addEdge(u, v);
            }
        }
    }

    BipartiteGraph& operator=(const BipartiteGraph& g) {
        if (&g == this) {
            return *this;
        }
        L = g.L;
        R = g.R;
        adj.clear();
        adj.resize(n);
        edges.clear();
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : g.adj[u]) {
                addEdge(u, v);
            }
        }
    }

    BipartiteGraph(BipartiteGraph&& g) noexcept :
    adj(std::move(g.adj)), edges(std::move(g.edges)), L(std::move(g.L)), R(std::move(g.R)) {}

    BipartiteGraph& operator=(BipartiteGraph&& g) noexcept {
        adj = std::move(g.adj);
        edges = std::move(g.edges);
        L = std::move(g.L);
        R = std::move(g.R);
        return *this;
    }

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        assert(L.contains(src) && R.contains(dst));
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
    }

    FlowNetwork<n + 2> getCorrespondingFlowNetwork() {
        FlowNetwork<n + 2> network;
        for (std::size_t u = 0; u < n; u++) {
            if (L.contains(u)) {
                network.addEdge(0, u + 1, 1);
            } else {
                assert(R.contains(u));
                network.addEdge(u + 1, n + 1, 1);
            }
            for (auto v : adj[u]) {
                network.addEdge(u + 1, v + 1, 1);
            }
        }
        return network;
    }

};

int main() {
    BipartiteGraph<9> g {0, 1, 2, 3, 4};
    g.addEdge(0, 5);
    g.addEdge(1, 5);
    g.addEdge(1, 7);
    g.addEdge(2, 6);
    g.addEdge(2, 7);
    g.addEdge(2, 8);
    g.addEdge(3, 7);
    g.addEdge(4, 7);
    auto f = g.getCorrespondingFlowNetwork();
    std::cout << f.PushRelabelBipartite() << '\n';



}