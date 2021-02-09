#include <algorithm>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <ranges>

namespace sr = std::ranges;

struct Graph {
    struct Edge {
        Edge(size_t source, size_t target, double weight) : source {source}, target {target}, weight {weight} {}
        size_t source;
        size_t target;
        double weight;
    };

    size_t vertices;
    std::vector<Edge> edges;

    explicit Graph (size_t n) : vertices {n} {}

    void addEdge(size_t source, size_t target, double weight) {
        assert(source < vertices && target < vertices && source != target && weight >= 0.0);
        edges.emplace_back(source, target, weight);
    }

    std::vector<Edge> maximalAcyclicEdges() {
        sr::sort(edges,
                  [](const auto &edge1, const auto &edge2) { return edge1.weight < edge2.weight; });
        std::vector<Edge> result;
        std::unordered_set<size_t> currVertices;
        for (const auto& [source, target, weight] : edges) {
            if (!currVertices.contains(source) && !currVertices.contains(target)) {
                currVertices.insert(source);
                currVertices.insert(target);
                result.emplace_back(source, target, weight);
            }
        }
        return result;
    }

};


int main() {
}
