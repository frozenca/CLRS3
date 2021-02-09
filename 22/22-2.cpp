#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <stack>
#include <ranges>

namespace sr = std::ranges;

template <std::size_t n>
class Graph {
    enum class Color {
        White,
        Gray,
        Black
    };
    struct DFSInfo {
        Color color = Color::White;
        std::size_t d = 0;
        std::size_t f = 0;
        std::size_t parent = -1;
        std::vector<std::size_t> children;
        std::size_t low = -1;
        std::vector<std::size_t> backedges;
    };

    std::vector<std::list<std::size_t>> adj;
    std::unordered_map<std::size_t, std::list<std::size_t>::iterator> edges;

    [[nodiscard]] bool isArticulationPoint(std::size_t u) const {
        assert(u < n);
        if (infos[u].parent == -1) {
            return infos[u].children.size() >= 2;
        } else {
            if (infos[u].d == infos[u].low && !infos[u].children.empty()) {
                return true;
            } else {
                return sr::any_of(infos[u].children,
                                           [u, this](auto ch) { return infos[ch].low > infos[u].d; });
            }
        }
    }
public:
    Graph() : adj(n), infos(n) {
    }

    std::vector<DFSInfo> infos;

    void addEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].push_front(dst);
            edges[src * n + dst] = adj[src].begin();
        }
        // for undirected
        if (!edges.contains(dst * n + src)) {
            adj[dst].push_front(src);
            edges[dst * n + src] = adj[dst].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        // for undirected
        adj[dst].erase(edges[dst * n + src]);
        edges.erase(dst * n + src);
    }

    void DFSVisit(std::size_t u, std::size_t& time, std::vector<std::size_t>& leaves) {
        time++;
        infos[u].d = time;
        infos[u].color = Color::Gray;
        bool is_leaf = true;
        for (auto v : adj[u]) {
            if (infos[v].color == Color::White) {
                infos[v].parent = u;
                infos[u].children.push_back(v);
                DFSVisit(v, time, leaves);
                is_leaf = false;
            } else if (sr::find(infos[v].children, u) == infos[v].children.end()){
                infos[u].backedges.push_back(v);
            }
        }
        if (is_leaf) {
            leaves.push_back(u);
        }
        infos[u].color = Color::Black;
        time++;
        infos[u].f = time;
    }

    void DFS() {
        infos = std::vector<DFSInfo>(n);
        std::size_t time = 0;
        std::vector<std::size_t> leaves;
        for (std::size_t i = 0; i < n; i++) {
            if (infos[i].color == Color::White) {
                DFSVisit(i, time, leaves);
            }
        }
        for (auto leaf : leaves) {
            std::size_t curr = leaf;
            infos[curr].low = std::min(infos[curr].low, infos[curr].d);
            while (curr != -1) {
                infos[curr].low = std::min(infos[curr].low, infos[curr].d);
                for (auto back : infos[curr].backedges) {
                    infos[curr].low = std::min(infos[curr].low, infos[back].d);
                }
                for (auto child : infos[curr].children) {
                    infos[curr].low = std::min(infos[curr].low, infos[child].low);
                }
                curr = infos[curr].parent;
            }
        }
    }

    [[nodiscard]] std::vector<std::size_t> getArticulationPoints() {
        DFS();
        std::vector<std::size_t> articulation_points;
        for (std::size_t i = 0; i < n; i++) {
            if (isArticulationPoint(i)) {
                articulation_points.push_back(i);
            }
        }
        return articulation_points;
    }

    [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> getBridges() {
        DFS();
        std::vector<std::pair<std::size_t, std::size_t>> bridges;
        for (std::size_t i = 0; i < n; i++) {
            if (infos[i].parent != -1 && infos[i].d == infos[i].low) {
                bridges.emplace_back(infos[i].parent, i);
            }
        }
        return bridges;
    }

    void BCCVisit(std::vector<std::vector<std::pair<std::size_t, std::size_t>>>& BCC,
                  std::size_t& bcc_index, std::size_t origin, std::size_t source, bool& quit) {
        infos[source].color = Color::Gray;
        while (true) {
            if (adj[source].empty() || quit) {
                break;
            }
            for (auto dest : adj[source]) {
                BCC[bcc_index].emplace_back(source, dest);
                removeEdge(source, dest);
                if (infos[dest].color == Color::White) {
                    BCCVisit(BCC, bcc_index, origin, dest, quit);
                }
                if (dest == origin) {
                    quit = true;
                }
                break;
            }
        }
    }

    [[nodiscard]] std::vector<std::vector<std::pair<std::size_t, std::size_t>>> getBiconnectedComponents() {
        auto articulation_points = getArticulationPoints();
        auto bridges = getBridges();
        Graph cloned;
        for (std::size_t u = 0; u < n; u++) {
            for (auto v : adj[u]) {
                cloned.addEdge(u, v);
            }
        }
        std::size_t bcc_index = 0;
        std::vector<std::vector<std::pair<std::size_t, std::size_t>>> BCC;
        for (const auto& [src, dst] : bridges) {
            BCC.emplace_back();
            BCC[bcc_index].emplace_back(src, dst);
            cloned.removeEdge(src, dst);
            bcc_index++;
        }
        for (std::size_t u = 0; u < n; u++) {
            if (cloned.infos[u].color == Color::White) {
                BCC.emplace_back();
                bool quit = false;
                cloned.BCCVisit(BCC, bcc_index, u, u, quit);
                if (BCC.back().empty()) {
                    BCC.pop_back();
                } else {
                    bcc_index++;
                }
                if (sr::find(articulation_points, u) != articulation_points.end()) {
                    cloned.infos[u].color = Color::White;
                }
            }
        }

        return BCC;
    }
};


int main() {
    Graph<23> g;
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(4, 6);
    g.addEdge(4, 7);
    g.addEdge(4, 8);
    g.addEdge(4, 10);
    g.addEdge(5, 6);
    g.addEdge(7, 8);
    g.addEdge(7, 9);
    g.addEdge(10, 11);
    g.addEdge(10, 12);
    g.addEdge(11, 13);
    g.addEdge(12, 13);
    g.addEdge(13, 14);
    g.addEdge(14, 16);
    g.addEdge(14, 17);
    g.addEdge(15, 16);
    g.addEdge(15, 17);
    g.addEdge(15, 18);
    g.addEdge(16, 17);
    g.addEdge(16, 19);
    g.addEdge(16, 20);
    g.addEdge(17, 18);
    g.addEdge(19, 21);
    g.addEdge(19, 22);
    g.addEdge(21, 22);
    auto articulation_points = g.getArticulationPoints();
    std::cout << "Articulation points: ";
    for (auto art_point : articulation_points) {
        std::cout << art_point << ' ';
    }
    std::cout << '\n';
    std::cout << "Bridges: ";
    auto bridges = g.getBridges();
    for (const auto& [src, dst] : bridges) {
        std::cout << '(' << src << ' ' << dst << ") ";
    }
    std::cout << '\n';
    std::cout << "Biconnected component indices:\n";
    auto bcc = g.getBiconnectedComponents();
    for (std::size_t i = 0; i < bcc.size(); i++) {
        std::cout << "BCC " << i << " : ";
        for (const auto& [src, dst] : bcc[i]) {
            std::cout << '(' << src << ", " << dst << ") ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';


}
