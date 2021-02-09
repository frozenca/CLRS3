#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <tuple>
#include <stack>
#include <list>

enum class Color {
    White,
    Gray,
    Black
};

using info_t = std::tuple<Color, std::size_t, std::size_t, std::size_t>;

template <std::size_t n>
class Graph {
    std::vector<std::vector<std::size_t>> adj;
public:
    Graph() : adj(n) {
    }

    std::vector<info_t> infos;

    std::size_t CountSimplePaths(std::size_t u, std::size_t v) {
        std::vector<std::size_t> numPaths(n, -1);
        return CountSimplePaths(u, v, numPaths);
    }

    std::size_t CountSimplePaths(std::size_t u, std::size_t v, std::vector<std::size_t>& numPaths) {
        if (u == v) {
            return 1;
        } else if (numPaths[u] != -1) {
            return numPaths[u];
        } else {
            numPaths[u] = 0;
            for (auto w : adj[u]) {
                numPaths[u] += CountSimplePaths(w, v, numPaths);
            }
        }
        return numPaths[u];
    }
};


int main() {

}
