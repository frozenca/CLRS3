#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <tuple>
#include <stack>

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

    bool isSinglyConnected() {
        infos = std::vector<info_t>(n, std::make_tuple(Color::White, 0, 0, -1));
        std::size_t time = 0;
        std::stack<std::size_t> S;
        for (std::size_t u = 0; u < n; u++) {
            if (std::get<0>(infos[u]) == Color::White) {
                S.push(u);
                while (!S.empty()) {
                    auto v = S.top();
                    time++;
                    std::get<0>(infos[v]) = Color::Gray;
                    std::get<1>(infos[v]) = time;
                    bool finished = true;
                    for (auto w : adj[v]) {
                        if (std::get<0>(infos[w]) == Color::White) {
                            S.push(w);
                            finished = false;
                        } else {
                            return false;
                        }
                    }
                    if (finished) {
                        S.pop();
                        time++;
                        std::get<0>(infos[v]) = Color::Black;
                        std::get<2>(infos[v]) = time;
                    }
                }
            }
        }
        return true;
    }
};


int main() {

}
