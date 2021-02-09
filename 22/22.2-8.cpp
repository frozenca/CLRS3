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
    Graph() : adj(n) {
        infos = std::vector<info_t>(n, std::make_tuple(Color::White, std::numeric_limits<std::size_t>::max(), -1));
    }

    std::vector<info_t> infos;

    std::size_t BFS(std::size_t source) {
        assert(source < n);
        infos = std::vector<info_t>(n, std::make_tuple(Color::White, std::numeric_limits<std::size_t>::max(), -1));
        infos[source] = {Color::Gray, 0, -1};
        std::queue<std::size_t> q;
        q.push(source);
        std::size_t last = -1;
        while (!q.empty()) {
            auto u = q.front();
            q.pop();
            for (auto v : adj[u]) {
                if (std::get<0>(infos[v]) == Color::White) {
                    infos[v] = {Color::Gray, std::get<1>(infos[u]) + 1, u};
                    q.push(v);
                    last = v;
                }
            }
            std::get<0>(infos[u]) = Color::Black;
        }
        return last;
    }

    std::size_t getDiameter() {
        auto u = BFS(0);
        auto v = BFS(u);
        return std::get<1>(infos[v]);
    }
};


int main() {

}
