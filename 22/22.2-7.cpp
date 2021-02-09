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

    void BFS(std::size_t source) {
        assert(source < n);
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
    }

    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> partite() {
        for (std::size_t i = 0; i < n; i++) {
            if (std::get<0>(infos[i]) == Color::White) {
                BFS(i);
            }
        }
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>> partition;
        for (std::size_t i = 0; i < n; i++) {
            if (std::get<1>(infos[i]) % 2 == 0) {
                partition.first.push_back(i);
            } else {
                partition.second.push_back(i);
            }
        }
        return partition;
    }
};


int main() {

}
