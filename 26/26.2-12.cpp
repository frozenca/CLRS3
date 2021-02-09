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
        if (!edges.contains(src * n + dst) && !edges.contains(dst * n + src)) {
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

    void dischargeFromFlow(std::size_t v) {
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
        for (std::size_t a = prev[v], b = v;
             a != -1 && b != source;
             b = a, a = prev[a]) {
            setFlow(a, b, getFlow(a, b) - 1);
        }
        setFlow(v, source, getFlow(v, source) - 1);
    }

};

int main() {


}