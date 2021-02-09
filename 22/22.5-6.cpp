#include <cassert>
#include <numeric>
#include <iostream>
#include <vector>
#include <utility>
#include <tuple>
#include <stack>
#include <list>
#include <algorithm>
#include <ranges>

namespace sr = std::ranges;

enum class Color {
    White,
    Gray,
    Black
};

using info_t = std::tuple<Color, std::size_t, std::size_t, std::size_t>;

std::vector<std::size_t> countingSort(const std::vector<std::size_t>& A, std::size_t k) {
    assert(!A.empty() && sr::all_of(A, [k](std::size_t n){return n < k;}));
    std::vector<std::size_t> C (k + 1);
    for (auto n : A) {
        C[n]++;
    }
    std::partial_sum(C.begin(), C.end(), C.begin());
    std::vector<std::size_t> B (A.size());
    for (std::size_t i = A.size() - 1; i < A.size(); i--) {
        C[A[i]]--;
        B[C[A[i]]] = A[i];
    }
    return B;
}

template <std::size_t n>
class Graph {
public:
    std::vector<std::vector<std::size_t>> adj;
    Graph() : adj(n) {
    }

    std::vector<info_t> infos;

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].push_back(dst);
    }

    [[nodiscard]] std::vector<std::size_t> getStronglyConnectedComponents() {
        infos = std::vector<info_t>(n, std::make_tuple(Color::White, 0, 0, -1));
        std::size_t time = 0;
        std::stack<std::size_t> S;
        std::list<std::size_t> toVisit;
        for (std::size_t u = 0; u < n; u++) {
            if (std::get<0>(infos[u]) == Color::White) {
                S.push(u);
                while (!S.empty()) {
                    auto v = S.top();
                    time++;
                    std::get<0>(infos[v]) = Color::Gray;
                    std::get<1>(infos[v]) = time;
                    bool finished = true;
                    for (auto w : adj[v]) {
                        if (std::get<0>(infos[w]) == Color::White) {
                            S.push(w);
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                        time++;
                        std::get<0>(infos[v]) = Color::Black;
                        std::get<2>(infos[v]) = time;
                        toVisit.push_front(v);
                    }
                }
            }
        }

        Graph transposed;

        for (std::size_t i = 0; i < n; i++) {
            for (auto neighbor : adj[i]) {
                transposed.adj[neighbor].push_back(i);
            }
        }

        std::vector<std::size_t> SCCMap (n);

        transposed.infos = std::vector<info_t>(n, std::make_tuple(Color::White, 0, 0, -1));
        time = 0;
        std::size_t SCCindex = 0;
        for (auto u : toVisit) {
            if (std::get<0>(transposed.infos[u]) == Color::White) {
                S.push(u);
                SCCMap[u] = SCCindex;
                while (!S.empty()) {
                    auto v = S.top();
                    time++;
                    std::get<0>(transposed.infos[v]) = Color::Gray;
                    std::get<1>(transposed.infos[v]) = time;
                    bool finished = true;
                    for (auto w : transposed.adj[v]) {
                        if (std::get<0>(transposed.infos[w]) == Color::White) {
                            S.push(w);
                            SCCMap[w] = SCCindex;
                            finished = false;
                        }
                    }
                    if (finished) {
                        S.pop();
                        time++;
                        std::get<0>(transposed.infos[v]) = Color::Black;
                        std::get<2>(transposed.infos[v]) = time;
                    }
                }
                SCCindex++;
            }
        }
        return SCCMap;
    }

    [[nodiscard]] Graph getComponentGraph() {
        auto SCCMap = getStronglyConnectedComponents();
        auto SortedSCCMap = countingSort(SCCMap, n);
        auto VSCC = SortedSCCMap;
        VSCC.erase(sr::unique(VSCC), VSCC.end());
        std::vector<std::pair<std::size_t, std::size_t>> S;
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : adj[u]) {
                if (SCCMap[u] != SCCMap[v]) {
                    S.emplace_back(SCCMap[u], SCCMap[v]);
                }
            }
        }

        // radix sort
        std::vector<std::size_t> C (n + 1);
        for (const auto& [u, v] : S) {
            C[v]++;
        }
        std::partial_sum(C.begin(), C.end(), C.begin());
        std::vector<std::pair<std::size_t, std::size_t>> B1 (S.size());
        for (std::size_t i = S.size() - 1; i < S.size(); i--) {
            C[S[i].second]--;
            B1[C[S[i].second]] = S[i];
        }
        sr::fill(C, 0);
        for (const auto& [u, v] : B1) {
            C[u]++;
        }
        std::partial_sum(C.begin(), C.end(), C.begin());
        std::vector<std::pair<std::size_t, std::size_t>> B2 (S.size());
        for (std::size_t i = S.size() - 1; i < S.size(); i--) {
            C[B1[i].first]--;
            B2[C[B1[i].first]] = B1[i];
        }
        B2.erase(sr::unique(B2), B2.end());

        Graph componentGraph;
        for (const auto& [u, v] : B2) {
            componentGraph.addEdge(u, v);
        }
        return componentGraph;

    }

    [[nodiscard]] Graph getCondensedGraph() {
        auto SCCMap = getStronglyConnectedComponents();
        auto cg = getComponentGraph();
        std::vector<std::vector<std::size_t>> SCCCycles(n);
        for (std::size_t i = 0; i < n; i++) {
            SCCCycles[SCCMap[i]].push_back(i);
        }

        Graph condensedGraph;

        // long cycle
        for (const auto& SCCCycle : SCCCycles) {
            if (SCCCycle.empty()) continue;
            condensedGraph.addEdge(SCCCycle.back(), SCCCycle.front());
            for (std::size_t i = 0; i < SCCCycle.size() - 1 && i < SCCCycle.size(); i++) {
                condensedGraph.addEdge(SCCCycle[i], SCCCycle[i + 1]);
            }
        }
        // component graph edges
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : cg.adj[u]) {
                condensedGraph.addEdge(SCCCycles[u][0], SCCCycles[v][0]);
            }
        }
        return condensedGraph;
    }

};

int main() {
    Graph<8> g;
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(1, 4);
    g.addEdge(1, 5);
    g.addEdge(2, 3);
    g.addEdge(2, 6);
    g.addEdge(3, 2);
    g.addEdge(3, 7);
    g.addEdge(4, 0);
    g.addEdge(4, 5);
    g.addEdge(5, 6);
    g.addEdge(6, 5);
    g.addEdge(7, 3);
    g.addEdge(7, 6);
    auto cd = g.getCondensedGraph();

    for (std::size_t u = 0; u < 8; u++) {
        for (auto v : cd.adj[u]) {
            std::cout << u << ' ' << v << '\n';
        }
    }


}
