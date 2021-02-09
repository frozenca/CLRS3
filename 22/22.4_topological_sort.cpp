#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <tuple>
#include <list>

template <std::size_t n>
class Graph {
    std::vector<std::vector<std::size_t>> adj;
public:
    Graph() : adj(n) {
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
};


int main() {

}
