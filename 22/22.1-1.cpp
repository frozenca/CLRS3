#include <vector>

class Graph {
    std::size_t n;
    std::vector<std::vector<std::size_t>> adj;
public:
    explicit Graph(std::size_t n) : n {n}, adj(n) {}

    [[nodiscard]] std::vector<std::size_t> getOutdegrees() const {
        std::vector<std::size_t> outdegrees(n);

        for (std::size_t i = 0; i < n; i++) {
            outdegrees[i] = adj[i].size();
        }
        return outdegrees;
    }

    [[nodiscard]] std::vector<std::size_t> getIndegrees() const {
        std::vector<std::size_t> indegrees(n);

        for (const auto& adjList : adj) {
            for (auto neighbor : adjList) {
                indegrees[neighbor]++;
            }
        }

        return indegrees;
    }

};


int main() {

}
