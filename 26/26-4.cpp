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

    void incrementEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return;
        }
        std::get<1>(*edges[src * n + dst]) += 1;
        setFlow(src, dst, getFlow(src, dst));
        auto [augmentable, argPathPrev] = getAugmentingPath(source, sink);
        if (!augmentable) {
            return;
        }
        Augment(argPathPrev, source, sink);
    }

    void decrementEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return;
        }
        std::get<1>(*edges[src * n + dst]) -= 1;
        if (getFlow(src, dst) == 0) {
            return;
        }
        setFlow(src, dst, getFlow(src, dst) - 1);

        auto [augmentable, argPathPrev] = getAugmentingPath(src, dst);
        if (augmentable) {
            Augment(argPathPrev, src, dst, 1);
        } else {
            auto [aug_us, path_us] = getAugmentingPath(src, source);
            assert(aug_us);
            Augment(path_us, src, source, 1);

            auto [aug_tv, path_tv] = getAugmentingPath(sink, dst);
            assert(aug_tv);
            Augment(path_tv, sink, dst, 1);
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

    std::pair<bool, std::vector<std::size_t>> getAugmentingPath(std::size_t src,
                                                                std::size_t dst) {
        std::vector<std::size_t> prev (n, -1);
        std::queue<std::size_t> q;
        q.push(src);
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
        return {prev[dst] != -1, prev};
    }

    [[nodiscard]] std::size_t getMaxFlow() const {
        std::size_t max_flow = 0;
        for (const auto& [v, c, f] : adj[source]) {
            max_flow += f;
        }
        return max_flow;
    }

    void Augment(const std::vector<std::size_t>& path,
                 std::size_t src, std::size_t dst,
                 std::size_t res_capacity = std::numeric_limits<std::size_t>::max()) {
        for (std::size_t u = path[dst], v = dst;
             u != -1 && v != src;
             v = u, u = path[u]) {
            res_capacity = std::min(res_capacity, getResidual(u, v));
        }

        for (std::size_t u = path[dst], v = dst;
             u != -1 && v != src;
             v = u, u = path[u]) {
            if (edges.contains(u * n + v)) {
                setFlow(u, v, getFlow(u, v) + res_capacity);
            } else {
                assert(edges.contains(v * n + u));
                setFlow(v, u, getFlow(v, u) - res_capacity);
            }
        }
    }

    void EdmondsKarp() {
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, c, f] : adj[u]) {
                setFlow(u, v, 0);
            }
        }
        while (true) {
            auto [augmentable, argPathPrev] = getAugmentingPath(source, sink);
            if (!augmentable) {
                break;
            }
            Augment(argPathPrev, source, sink);
        }
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
    std::cout << network.getMaxFlow() << '\n';
    network.incrementEdge(3, 2);
    std::cout << network.getMaxFlow() << '\n';
    network.incrementEdge(1, 3);
    std::cout << network.getMaxFlow() << '\n';
    network.decrementEdge(1, 3);
    std::cout << network.getMaxFlow() << '\n';
    network.decrementEdge(3, 2);
    std::cout << network.getMaxFlow() << '\n';

}