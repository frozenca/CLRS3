#include <array>
#include <vector>
#include <utility>

template <std::size_t n>
class GraphList {
    std::vector<std::vector<std::size_t>> adj;
public:
    GraphList() : adj(n) {}

    [[nodiscard]] GraphList square() const {
        GraphList transposed;

        for (std::size_t i = 0; i < n; i++) {
            for (auto neighbor : adj[i]) {
                transposed.adj[neighbor].push_back(i);
            }
        }

        GraphList squared;

        for (std::size_t i = 0; i < n; i++) {
            for (auto neighbor : adj[i]) {
                squared.adj[i].push_back(neighbor);
                for (auto immediate : transposed.adj[neighbor]) {
                    squared.adj[i].push_back(immediate);
                }
            }
        }

        return squared;
    }
};

template <std::size_t n>
class GraphMatrix {
    std::array<std::size_t, n * n> adj;
public:
    [[nodiscard]] GraphMatrix square() const {
        GraphMatrix squared;
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < n; j++) {
                for (std::size_t k = 0; k < n; k++) {
                    squared.adj[i * n + j] += adj[i * n + k] * adj[k * n + j];
                }
            }
        }
        return squared;
    }
};



int main() {

}
