#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <tuple>

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

    void DFSVisit(std::size_t u, std::size_t& time, std::vector<std::size_t>& path) {
        time++;
        std::get<1>(infos[u]) = time;
        path.push_back(u);
        std::get<0>(infos[u]) = Color::Gray;
        for (auto v : adj[u]) {
            if (std::get<0>(infos[v]) == Color::White) {
                std::get<3>(infos[v]) = u;
                DFSVisit(v, time);
            }
        }
        std::get<0>(infos[u]) = Color::Black;
        time++;
        std::get<2>(infos[u]) = time;
        path.push_back(u);
    }

    void DFS() {
        infos = std::vector<info_t>(n, std::make_tuple(Color::White, 0, 0, -1));
        std::size_t time = 0;
        std::vector<std::size_t> path;
        for (std::size_t i = 0; i < n; i++) {
            if (std::get<0>(infos[i]) == Color::White) {
                DFSVisit(i, time, path);
            }
        }
        for (auto vertex : path) {
            std::cout << vertex << ' ';
        }
    }
};


int main() {

}
