#include <limits>
#include <iostream>
#include <vector>
#include <stack>

using DirectedEdge = std::pair<size_t, double>;

struct Graph {
    size_t V;
    std::vector<std::vector<DirectedEdge>> adj;

    explicit Graph (size_t V) : V {V} {
        adj.resize(V);
    }

    void addEdge(size_t src, size_t dst, double weight) {
        adj[src].emplace_back(dst, weight);
    }

    void topologicalSortHelper(size_t v, std::vector<int>& visited, std::stack<size_t>& st) {
        visited[v] = 1;

        for (const auto& [dst, w] : adj[v]) {
            if (!visited[dst]) {
                topologicalSortHelper(dst, visited, st);
            }
        }

        st.push(v);
    }

    double longestPath(size_t src, size_t dst) {
        std::stack<size_t> st;
        std::vector<double> dist (V, std::numeric_limits<double>::lowest());
        std::vector<int> visited (V);

        for (size_t i = 0; i < V; i++) {
            if (!visited[i]) {
                topologicalSortHelper(i, visited, st);
            }
        }
        dist[src] = 0;

        while (!st.empty()) {
            size_t curr = st.top();
            st.pop();

            if (dist[curr] != std::numeric_limits<double>::lowest()) {
                for (const auto& [next, w] : adj[curr]) {
                    dist[next] = std::max(dist[next], dist[curr] + w);
                }
            }
        }

        return dist[dst];
    }

};


int main() {
    Graph g (6);
    g.addEdge(0, 1, 5.0);
    g.addEdge(0, 2, 3.0);
    g.addEdge(1, 2, 2.0);
    g.addEdge(1, 3, 6.0);
    g.addEdge(2, 3, 7.0);
    g.addEdge(2, 4, 4.0);
    g.addEdge(2, 5, 2.0);
    g.addEdge(3, 4, -1.0);
    g.addEdge(3, 5, 1.0);
    g.addEdge(4, 5, -2.0);

    std::cout << g.longestPath(1, 5) << '\n';


}