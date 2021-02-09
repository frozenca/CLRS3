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
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace crn = std::chrono;

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
            if (v != source && height[u] > height[v]) {
                return false;
            }
        }
        return true;
    }

    void Relabel(std::size_t u, std::vector<std::size_t>& height) {
        height[u]++;
    }

    void Discharge(std::size_t v, std::vector<int>& excess, std::vector<std::size_t>& height,
                   std::vector<std::list<std::size_t>::iterator>& current_edges,
                   std::vector<std::unordered_set<std::size_t>>& height_sets) {
        while (excess[v] > 0) {
            if (current_edges[v] == undirected_adj[v].end()) {
                assert(Relabelable(v, excess, height));
                height_sets[height[v]].erase(v);
                Relabel(v, height);
                height_sets[height[v]].insert(v);
                current_edges[v] = undirected_adj[v].begin();
            } else if (Pushable(v, *current_edges[v], excess, height)) {
                Push(v, *current_edges[v], excess);
            } else {
                std::advance(current_edges[v], 1);
            }
        }
    }

    std::size_t RelabelToFront() {
        std::vector<int> excess (n);
        std::vector<std::size_t> height (n);
        std::vector<std::list<std::size_t>::iterator> current_edges(n);
        InitializePreflow(excess, height, current_edges);
        std::vector<std::unordered_set<std::size_t>> height_sets (2 * n);

        std::list<std::size_t> L;
        for (std::size_t u = 0; u < n; u++) {
            if (u != source && u != sink) {
                L.push_back(u);
                height_sets[0].insert(u);
            }
        }

        auto Lp = L.begin();
        std::size_t count = 0;
        while (Lp != L.end()) {
            auto u = *Lp;
            auto old_height = height[u];
            Discharge(u, excess, height, current_edges, height_sets);
            if (height[u] > old_height) {
                L.erase(Lp);
                L.push_front(u);
                Lp = L.begin();
            } else {
                ++Lp;
            }
            count++;
            if (count > n * n) {
                std::size_t k = 0;
                for (std::size_t v = 1; v < n; v++) {
                    if (height_sets[v].empty()) {
                        k = v;
                        break;
                    }
                }
                if (k) {
                    for (std::size_t v = k + 1; v <= n; v++) {
                        for (auto num : height_sets[v]) {
                            height[num] = n + 1;
                        }
                        height_sets[n + 1].merge(height_sets[v]);
                    }
                }
                count = 0;
            }
        }

        std::size_t max_flow = 0;
        for (const auto& [v, c, f] : adj[source]) {
            max_flow += f;
        }
        return max_flow;
    }
};

std::mt19937 gen(std::random_device{}());
constexpr std::size_t V = 300;
std::uniform_int_distribution<> dist(1, 100);
constexpr double density = 0.5;
std::bernoulli_distribution make_edge(density);

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
    assert(network.EdmondsKarp() == network.RelabelToFront());

    FlowNetwork<4> network2;
    network2.addEdge(0, 1, 1000000);
    network2.addEdge(0, 2, 1000000);
    network2.addEdge(1, 3, 1000000);
    network2.addEdge(2, 3, 1000000);
    network2.addEdge(1, 2, 1);
    assert(network2.EdmondsKarp() == network2.RelabelToFront());

    FlowNetwork<V> network3;
    for (std::size_t i = 1; i < V - 1; i++) {
        network3.addEdge(0, i, dist(gen));
        network3.addEdge(i, V - 1, dist(gen));
        for (std::size_t j = 1; j < V - 1; j++) {
            if (i != j && make_edge(gen)) {
                network3.addEdge(i, j, dist(gen));
            }
        }
    }

    auto t1 = crn::steady_clock::now();
    std::cout << network3.EdmondsKarp() << '\n';
    auto t2 = crn::steady_clock::now();
    auto dt1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << "Edmonds-Karp: " << dt1.count() << "ms\n";
    auto t3 = crn::steady_clock::now();
    std::cout << network3.RelabelToFront() << '\n';
    auto t4 = crn::steady_clock::now();
    auto dt2 = crn::duration_cast<crn::milliseconds>(t4 - t3);
    std::cout << "Relabel-To-Front: " << dt2.count() << "ms\n";

}