#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cmath>
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

    BipartiteGraph(const std::vector<std::size_t>& L_) : adj(n), L (L_.begin(), L_.end()) {
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

    BipartiteGraph(BipartiteGraph&& g) noexcept = default;
    BipartiteGraph& operator=(BipartiteGraph&& g) noexcept = default;

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        assert(L.contains(src) && R.contains(dst));
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
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

    static constexpr std::size_t INF = std::numeric_limits<std::size_t>::max();
    static constexpr std::size_t NIL = n;

    bool hasAugmentingPath(const std::vector<std::size_t>& paired,
                           std::vector<std::size_t>& level) {
        std::queue<std::size_t> Q;
        for (auto l : L) {
            if (paired[l] == NIL) {
                level[l] = 0;
                Q.push(l);
            } else {
                level[l] = INF;
            }
        }
        level[NIL] = INF;
        while (!Q.empty()) {
            auto l = Q.front();
            Q.pop();
            if (level[l] < level[NIL]) {
                for (auto r : adj[l]) {
                    auto next_l = paired[r];
                    if (level[next_l] == INF) {
                        level[next_l] = level[l] + 1;
                        Q.push(next_l);
                    }
                }
            }
        }
        return level[NIL] != INF;
    }

    bool Augment(std::size_t l,
                        std::vector<std::size_t>& paired,
                        std::vector<std::size_t>& level) {
        if (l != NIL) {
            for (auto r : adj[l]) {
                if (level[paired[r]] == level[l] + 1) {
                    if (Augment(paired[r], paired, level)) {
                        paired[r] = l;
                        paired[l] = r;
                        return true;
                    }
                }
            }
            level[l] = INF;
            return false;
        }
        return true;
    }

    std::size_t HopcroftKarp() {
        std::vector<std::size_t> paired (n + 1, NIL);
        std::vector<std::size_t> level (n + 1, INF);
        std::size_t matching = 0;

        while (hasAugmentingPath(paired, level)) {
            for (auto l : L) {
                if (paired[l] == NIL) {
                    if (Augment(l, paired, level)) {
                        matching++;
                    }
                }
            }
        }
        return matching;
    }
};

std::mt19937 gen(std::random_device{}());
constexpr std::size_t V = 2'000;
constexpr double density = 0.2;
std::bernoulli_distribution make_edge(density);

int main() {
    std::vector<std::size_t> temp;
    for (std::size_t i = 0; i < V; i++) {
        temp.push_back(i);
    }
    BipartiteGraph<2 * V> bipartite(temp);
    for (std::size_t i = 0; i < V; i++) {
        for (std::size_t j = std::max(V, V + i - 10); j < std::min(2 * V - 1, V + i + 10); j++) {
            if (make_edge(gen)) {
                bipartite.addEdge(i, j);
            }
        }
    }
    auto flow_network = bipartite.getCorrespondingFlowNetwork();
    auto t1 = crn::steady_clock::now();
    std::cout << flow_network.RelabelToFront() << '\n';
    auto t2 = crn::steady_clock::now();
    auto dt1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << "Relabel-To-Front: " << dt1.count() << "ms\n";
    auto t3 = crn::steady_clock::now();
    std::cout << bipartite.HopcroftKarp() << '\n';
    auto t4 = crn::steady_clock::now();
    auto dt2 = crn::duration_cast<crn::milliseconds>(t4 - t3);
    std::cout << "Hopcroft-Karp: " << dt2.count() << "ms\n";


}