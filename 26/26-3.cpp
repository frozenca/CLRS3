#include <algorithm>
#include <cassert>
#include <cstddef>
#include <chrono>
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
    FlowNetwork(std::size_t source = 0, std::size_t sink = n - 1) : adj(n),
                                                                    undirected_adj(n), res(n), source (source), sink (sink) {}

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

        undirected_adj.clear();
        undirected_adj.resize(n);
        undirected_edges.clear();

        res.clear();
        res.resize(n);
        res_edges.clear();

        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c] : g.adj[u]) {
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

    void Discharge(std::size_t v, std::vector<int>& excess, std::vector<std::size_t>& height,
                   std::vector<std::list<std::size_t>::iterator>& current_edges) {
        while (excess[v] > 0) {
            if (current_edges[v] == undirected_adj[v].end()) {
                assert(Relabelable(v, excess, height));
                Relabel(v, height);
                current_edges[v] = undirected_adj[v].begin();
            } else if (Pushable(v, *current_edges[v], excess, height)) {
                Push(v, *current_edges[v], excess);
            } else {
                std::advance(current_edges[v], 1);
            }
        }
    }

    std::unordered_set<std::size_t> RelabelToFront() {
        std::vector<int> excess (n);
        std::vector<std::size_t> height (n);
        std::vector<std::list<std::size_t>::iterator> current_edges(n);
        InitializePreflow(excess, height, current_edges);

        std::list<std::size_t> L;
        for (std::size_t u = 0; u < n; u++) {
            if (u != source && u != sink) {
                L.push_back(u);
            }
        }
        auto Lp = L.begin();
        while (Lp != L.end()) {
            auto u = *Lp;
            auto old_height = height[u];
            Discharge(u, excess, height, current_edges);
            if (height[u] > old_height) {
                L.erase(Lp);
                L.push_front(u);
                Lp = L.begin();
            } else {
                ++Lp;
            }
        }

        std::vector<int> possible_heights (n + 3, 1);
        for (auto h : height) {
            possible_heights[h] = 0;
        }
        auto H = std::distance(possible_heights.begin(), sr::find(possible_heights, 1));

        std::unordered_set<std::size_t> S, T;
        for (std::size_t u = 0; u < n; u++) {
            if (height[u] > H) {
                S.insert(u);
            } else {
                T.insert(u);
            }
        }

        return T;
    }
};

constexpr std::size_t INF = 1'000'000;

int main() {
    constexpr std::size_t N = 5;
    constexpr std::size_t M = 4;

    FlowNetwork<N + M + 2> network;
    // 0 source, 1~5 A, 6~9 J, 10 sink
    network.addEdge(0, 1, 10);
    network.addEdge(0, 2, 20);
    network.addEdge(0, 3, 30);
    network.addEdge(0, 4, 40);
    network.addEdge(0, 5, 50);
    network.addEdge(6, 10, 60);
    network.addEdge(7, 10, 70);
    network.addEdge(8, 10, 80);
    network.addEdge(9, 10, 90);
    network.addEdge(1, 6, INF);
    network.addEdge(2, 6, INF);
    network.addEdge(2, 8, INF);
    network.addEdge(3, 7, INF);
    network.addEdge(3, 8, INF);
    network.addEdge(3, 9, INF);
    network.addEdge(4, 8, INF);
    network.addEdge(5, 8, INF);

    auto T = network.RelabelToFront();
    std::cout << "Expert to hire: ";
    for (std::size_t i = 1; i <= N; i++) {
        if (T.contains(i)) {
            std::cout << i << ' ';
        }
    }
    std::cout << '\n';
    std::cout << "Job to accept: ";
    for (std::size_t i = N + 1; i <= N + M; i++) {
        if (T.contains(i)) {
            std::cout << i << ' ';
        }
    }
    std::cout << '\n';




}