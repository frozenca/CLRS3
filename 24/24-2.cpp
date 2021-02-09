#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <tuple>
#include <concepts>
#include <limits>
#include <ranges>
#include <stack>
#include <random>
#include <array>

namespace sr = std::ranges;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <std::size_t n, arithmetic T = double, bool undirected = false>
class Graph {
    std::vector<std::list<std::pair<std::size_t, T>>> adj;
    std::unordered_map<std::size_t, typename std::list<std::pair<std::size_t, T>>::iterator> edges;

    struct PathInfo {
        T d = std::numeric_limits<T>::max();
        std::size_t parent = -1;
    };

public:
    Graph() : adj(n), pinfos(n) {
    }

    std::vector<PathInfo> pinfos;

    void addEdge(std::size_t src, std::size_t dst, T weight) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, weight);
            edges[src * n + dst] = adj[src].begin();
        }
        if (undirected) {
            if (!edges.contains(dst * n + src)) {
                adj[dst].emplace_front(src, weight);
                edges[dst * n + src] = adj[dst].begin();
            }
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        if (undirected) {
            adj[dst].erase(edges[dst * n + src]);
            edges.erase(dst * n + src);
        }
    }

    [[nodiscard]] T getWeight(std::size_t src, std::size_t dst) const {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return T {0};
        } else {
            return edges.at(src * n + dst)->second;
        }
    }

    using Edge = std::tuple<std::size_t, std::size_t, T>;

    void InitializeSingleSource(std::size_t s) {
        pinfos.resize(0);
        pinfos.resize(n);
        pinfos[s].d = T {0};
    }

    void Relax(std::size_t u, std::size_t v) {
        T w = getWeight(u, v);
        if (pinfos[v].d > pinfos[u].d + w) {
            pinfos[v].d = pinfos[u].d + w;
            pinfos[v].parent = u;
        }
    }

    void DFSVisit(std::size_t u, std::vector<int>& visited, std::list<std::size_t>& topSort) {
        visited[u] = true;
        for (const auto& [v, w] : adj[u]) {
            if (!visited[v]) {
                DFSVisit(v, visited, topSort);
            }
        }
        topSort.push_front(u);
    }

    std::list<std::size_t> TopologicalSort() {
        std::stack<std::size_t> S;
        std::list<std::size_t> topSort;
        std::vector<int> visited (n);

        for (std::size_t u = 0; u < n; u++) {
            if (!visited[u]) {
                DFSVisit(u, visited, topSort);
            }
        }
        return topSort;
    }

    void NegateWeights() {
        for (std::size_t i = 0; i < n; i++) {
            for (auto& [j, w] : adj[i]) {
                w = -w;
            }
        }
    }

    std::list<std::size_t> DAGLongestPaths(std::size_t s) {
        auto TS = TopologicalSort();
        InitializeSingleSource(s);
        NegateWeights();
        for (auto u : TS) {
            for (const auto& [v, w] : adj[u]) {
                Relax(u, v);
            }
        }

        std::list<std::size_t> path;
        storePath(s, n - 1, path);
        path.pop_front();
        path.pop_back();
        return path;
    }

    void storePath(std::size_t source, std::size_t dest, std::list<std::size_t>& path) {
        if (dest == source) {
            path.push_front(source);
        } else if (pinfos[dest].parent == -1) {
        } else {
            storePath(source, pinfos[dest].parent, path);
            path.push_back(dest);
        }
    }
};

std::mt19937 gen(std::random_device{}());
std::uniform_int_distribution<> dist(1, 100);

constexpr std::size_t num_dimension = 5;
using Box = std::array<std::size_t, num_dimension>;

constexpr std::size_t num_boxes = 200;

std::vector<Box> generateBoxes() {
    std::vector<Box> boxes;
    for (std::size_t i = 0; i < num_boxes; i++) {
        Box b;
        for (std::size_t j = 0; j < num_dimension; j++) {
            b[j] = dist(gen);
        }
        boxes.push_back(b);
    }
    return boxes;
}

int doesNest(const Box& b1, const Box& b2) {
    bool b1_nests_b2 = true;
    bool b2_nests_b1 = true;
    for (std::size_t i = 0; i < num_dimension; i++) {
        if (b1[i] > b2[i]) {
            b1_nests_b2 = false;
        } else if (b1[i] < b2[i]) {
            b2_nests_b1 = false;
        } else {
            b1_nests_b2 = false;
            b2_nests_b1 = false;
        }
    }
    if (b1_nests_b2) {
        return 1;
    } else if (b2_nests_b1) {
        return -1;
    } else {
        return 0;
    }
}

Graph<num_boxes + 2> getGraphFromBoxes(std::vector<Box>& boxes) {
    assert(boxes.size() == num_boxes);
    for (std::size_t i = 0; i < num_boxes; i++) {
        sr::sort(boxes[i]);
        std::cout << "Box " << i << " : (";
        for (auto coord : boxes[i]) {
            std::cout << coord << ' ';
        }
        std::cout << ")\n";
    }

    Graph<num_boxes + 2> g;

    std::vector<int> zero_indegrees (num_boxes, 1);
    std::vector<int> zero_outdegrees (num_boxes, 1);

    for (std::size_t i = 0; i < num_boxes; i++) {
        for (std::size_t j = i + 1; j < num_boxes; j++) {
            int result = doesNest(boxes[i], boxes[j]);
            if (result == 1) {
                g.addEdge(i, j, 1);
                zero_indegrees[j] = 0;
                zero_outdegrees[i] = 0;
            } else if (result == -1) {
                g.addEdge(j, i, 1);
                zero_indegrees[i] = 0;
                zero_outdegrees[j] = 0;
            }
        }
    }
    for (std::size_t i = 0; i < num_boxes; i++) {
        // supersource
        if (zero_indegrees[i]) {
            g.addEdge(num_boxes, i, 1);
        }
        // supersink
        if (zero_outdegrees[i]) {
            g.addEdge(i, num_boxes + 1, 1);
        }
    }
    return g;
}

int main() {

    auto boxes = generateBoxes();
    auto g = getGraphFromBoxes(boxes);
    std::cout << "Longest nested sequence of boxes: ";
    auto path = g.DAGLongestPaths(num_boxes);
    for (auto v : path) {
        std::cout << v << ' ';
    }
}