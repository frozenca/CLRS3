#include <array>
#include <vector>
#include <utility>

template <std::size_t n>
class Graph {
    std::vector<std::vector<std::size_t>> adj;
public:
    Graph() : adj(n) {}

    [[nodiscard]] Graph transpose() const {
        Graph transposed;

        for (std::size_t i = 0; i < n; i++) {
            for (auto neighbor : adj[i]) {
                transposed.adj[neighbor].push_back(i);
            }
        }
        return transposed;
    }
};

template <std::size_t n>
class GraphM {
    std::array<std::size_t, n * n> adj;
public:
    [[nodiscard]] GraphM transpose() const {
        GraphM transposed;
        transposed.adj = adj;
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < i; j++) {
                std::swap(transposed.adj[i * n + j], transposed.adj[j * n + i]);
            }
        }
        return transposed;
    }
};


int main() {

}
