#include <array>
#include <vector>
#include <utility>

template <std::size_t n>
class Graph {
    std::vector<std::vector<std::size_t>> adj;
public:
    Graph() : adj(n) {}

    [[nodiscard]] Graph getReduced() const {
        Graph reduced;

        for (std::size_t i = 0; i < n; i++) {
            std::vector<std::size_t> aux (n);
            for (auto neighbor : adj[i]) {
                if (neighbor != i) {
                    if (aux[neighbor] != 1) {
                        aux[neighbor] = 1;
                        reduced.adj[i].push_back(neighbor);
                    }
                }
            }
        }
        return reduced;
    }
};


int main() {

}
