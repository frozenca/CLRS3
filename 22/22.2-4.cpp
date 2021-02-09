#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <utility>
#include <tuple>
#include <queue>

enum class Color {
    White,
    Gray,
    Black
};

using info_t = std::tuple<Color, std::size_t, std::size_t>;

template <std::size_t n>
class Graph {
    std::vector<std::vector<std::size_t>> adj;
public:
    Graph() : adj(n) {}

    std::vector<info_t> BFS(std::size_t source) {
        assert(source < n);
        std::vector<info_t> infos (n, std::make_tuple(Color::White, std::numeric_limits<std::size_t>::max(), -1));
        infos[source] = {Color::Gray, 0, -1};
        std::queue<std::size_t> q;
        q.push(source);
        while (!q.empty()) {
            auto u = q.front();
            q.pop();
            for (auto v : adj[u]) {
                if (std::get<0>(infos[v]) == Color::White) {
                    infos[v] = {Color::Gray, std::get<1>(infos[u]) + 1, u};
                    q.push(v);
                }
            }
            std::get<0>(infos[u]) = Color::Black;
        }
        return infos;
    }

    void printPath(std::size_t source, std::size_t dest, const std::vector<info_t>& infos) {
        if (dest == source) {
            std::cout << source;
        } else if (std::get<2>(infos[dest]) == -1) {
            std::cout << "No path from " << source << " to " << dest << " exists\n";
        } else {
            printPath(source, std::get<2>(infos[dest]), infos);
        }
    }
};

template <std::size_t n>
class GraphM {
    std::array<std::size_t, n * n> adj;
public:

    std::vector<info_t> BFS(std::size_t source) {
        assert(source < n);
        std::vector<info_t> infos (n, std::make_tuple(Color::White, std::numeric_limits<std::size_t>::max(), -1));
        infos[source] = {Color::Gray, 0, -1};
        std::queue<std::size_t> q;
        q.push(source);
        while (!q.empty()) {
            auto u = q.front();
            q.pop();
            for (std::size_t v = 0; v < n; v++) {
                if (adj[u * n + v] == 1 && v != u && std::get<0>(infos[v]) == Color::White) {
                    infos[v] = {Color::Gray, std::get<1>(infos[u]) + 1, u};
                    q.push(v);
                }
            }
            std::get<0>(infos[u]) = Color::Black;
        }
        return infos;
    }

    void printPath(std::size_t source, std::size_t dest, const std::vector<info_t>& infos) {
        if (dest == source) {
            std::cout << source;
        } else if (std::get<2>(infos[dest]) == -1) {
            std::cout << "No path from " << source << " to " << dest << " exists\n";
        } else {
            printPath(source, std::get<2>(infos[dest]), infos);
        }
    }
};

int main() {

}
