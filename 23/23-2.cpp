#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <tuple>
#include <memory>
#include <algorithm>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <limits>
#include <map>
#include <set>

namespace sr = std::ranges;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept hashable = requires (T a) {
    {std::hash<T>{}(a)} -> std::convertible_to<std::size_t>;
};

template <arithmetic T, hashable V>
class FibonacciHeap {
    struct Node {
        Node* left = this;
        Node* right = this;
        Node* parent = nullptr;
        Node* child = nullptr;
        std::size_t degree = 0;
        bool mark = false;
        T key;
        V value;

        Node(const T& key, V value) : key {key}, value {std::move(value)} {}
    };

    Node* min = nullptr;
    std::size_t n = 0;
    std::unordered_set<Node*> nodes;
    std::unordered_map<V, Node*> nodes_by_index;

    void addToRoot(Node* x) {
        assert(x);
        if (!min) {
            min = x;
            x->right = x;
            x->left = x;
        } else {
            auto r = min->right;
            min->right = x;
            r->left = x;
            x->left = min;
            x->right = r;
        }
        if (x->key < min->key) {
            min = x;
        }
        x->parent = nullptr;
    }

    void FibHeapLink(Node* y, Node* x) {
        y->right->left = y->left;
        y->left->right = y->right;
        auto c = x->child;
        if (!c) {
            x->child = y;
            y->left = y;
            y->right = y;
        } else {
            c->right->left = y;
            y->right = c->right;
            c->right = y;
            y->left = c;
        }
        y->parent = x;
        y->right->parent = x;
        y->left->parent = x;
        y->mark = false;
        x->degree++;
    }

    void Consolidate() {
        std::vector<Node*> A (n + 1, nullptr);
        std::vector<Node*> roots;
        auto curr = min;
        if (curr) {
            do {
                roots.push_back(curr);
                curr = curr->right;
            } while (curr != min);
        }
        for (auto w : roots) {
            auto x = w;
            auto d = x->degree;
            while (A[d]) {
                auto y = A[d];
                if (x->key > y->key) {
                    std::swap(x, y);
                }
                FibHeapLink(y, x);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }
        min = nullptr;
        for (std::size_t i = 0; i <= n; i++) {
            if (A[i]) {
                addToRoot(A[i]);
            }
        }
    }

    void Cut(Node* x, Node* y) {
        assert(x->parent == y);
        x->right->left = x->left;
        x->left->right = x->right;
        if (y->child == x) {
            if (x == x->right) {
                y->child = nullptr;
            } else {
                y->child = x->right;
            }
        }
        y->degree--;
        addToRoot(x);
        x->mark = false;
    }

    void CascadingCut(Node* y) {
        if (!y) {
            return;
        }
        auto z = y->parent;
        if (z) {
            if (!y->mark) {
                y->mark = true;
            } else {
                Cut(y, z);
                CascadingCut(z);
            }
        }
    }

public:
    ~FibonacciHeap() {
        for (auto node : nodes) {
            delete node;
        }
    }

    [[nodiscard]] bool isEmpty() const {
        return nodes.empty();
    }

    void Insert(const T& key, const V& value) {
        auto x = new Node(key, value);
        nodes.insert(x);
        nodes_by_index[value] = x;
        addToRoot(x);
        n++;
    }

    [[nodiscard]] Node* getMinimum() const {
        return min;
    }

    [[nodiscard]] Node* SearchByValue(const V& value) const {
        if (nodes_by_index.contains(value)) {
            return nodes_by_index.find(value)->second;
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] std::unique_ptr<Node> ExtractMin() {
        auto z = min;
        if (z) {
            std::vector<Node*> children;
            auto curr = z->child;
            if (curr) {
                do {
                    children.push_back(curr);
                    curr = curr->right;
                } while (curr != z->child);
            }
            for (auto x : children) {
                addToRoot(x);
            }
            z->right->left = z->left;
            z->left->right = z->right;
            nodes.erase(z);
            nodes_by_index.erase(z->value);
            n--;
            if (z == z->right) {
                min = nullptr;
            } else {
                min = z->right;
                Consolidate();
            }
        }
        return std::unique_ptr<Node>(z);
    }

    friend FibonacciHeap Union(FibonacciHeap& H1, FibonacciHeap& H2) {
        FibonacciHeap H;
        if (!H1.min) {
            H.min = H2.min;
        } else if (!H2.min) {
            H.min = H1.min;
        } else {
            H.min = (H1.min->key < H2.min->key) ? H1.min : H2.min;
            auto H1head = H1.min;
            auto H1tail = H1.min->left;
            auto H2head = H2.min;
            auto H2tail = H2.min->left;
            H1tail->right = H2head;
            H2head->left = H1tail;
            H2tail->right = H1head;
            H1head->left = H2tail;
        }
        std::swap(H.nodes, H1.nodes);
        sr::move(H2.nodes, H.nodes.end());
        H.n = H1.n + H2.n;
        return H;
    }

    void DecreaseKey(Node* x, const T& k) {
        if (!x || k > x->key) {
            return;
        }
        x->key = k;
        auto y = x->parent;
        if (y && x->key < y->key) {
            Cut(x, y);
            CascadingCut(y);
        }
        if (x->key < min->key) {
            min = x;
        }
    }

    void Delete(Node* x) {
        DecreaseKey(x, std::numeric_limits<T>::lowest());
        auto m = ExtractMin();
    }
};

template <std::size_t n>
class Graph {
    struct UnionInfo {
        std::size_t index;
        std::size_t parent;
        std::size_t rank = 0;
        UnionInfo(std::size_t index) : index {index}, parent {index} {}
    };

    void MakeSet (std::size_t v) {
        assert(v < n);
        uinfos[v] = UnionInfo{v};
    }

    std::vector<std::list<std::pair<std::size_t, double>>> adj;
    std::unordered_map<std::size_t, std::list<std::pair<std::size_t, double>>::iterator> edges;

    void Link(std::size_t x, std::size_t y) {
        auto& ux = uinfos[x];
        auto& uy = uinfos[y];
        if (ux.rank < uy.rank) {
            uy.parent = x;
        } else {
            ux.parent = y;
            if (ux.rank == uy.rank) {
                uy.rank++;
            }
        }
    }

    std::size_t FindSet(std::size_t x) {
        auto& ux = uinfos[x];
        if (ux.index != ux.parent) {
            ux.parent = FindSet(ux.parent);
        }
        return ux.parent;
    }

    void Union(std::size_t x, std::size_t y) {
        Link(FindSet(x), FindSet(y));
    }

public:
    Graph() : adj(n) {
        for (std::size_t i = 0; i < n; i++) {
            uinfos.emplace_back(i);
        }
    }

    std::vector<UnionInfo> uinfos;

    [[nodiscard]] bool hasEdge(std::size_t src, std::size_t dst) const {
        assert(src < n && dst < n);
        return edges.contains(src * n + dst);
    }

    [[nodiscard]] double getEdgeWeight(std::size_t src, std::size_t dst) const {
        if (hasEdge(src, dst)) {
            return edges.at(src * n + dst)->second;
        } else {
            return 0.0;
        }
    }

    void setEdgeWeight(std::size_t src, std::size_t dst, double weight) {
        addEdge(src, dst, weight);
        addEdge(src, dst, weight);
    }

    void addEdge(std::size_t src, std::size_t dst, double weight) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, weight);
            edges[src * n + dst] = adj[src].begin();
        }
        if (!edges.contains(dst * n + src)) {
            adj[dst].emplace_front(src, weight);
            edges[dst * n + src] = adj[dst].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        adj[dst].erase(edges[dst * n + src]);
        edges.erase(dst * n + src);
    }

    // (src, dst, weight)

    using Edge = std::tuple<std::size_t, std::size_t, double>;

    std::vector<Edge> MSTPrim(std::size_t r) {
        std::vector<Edge> A;

        // (w, (u, parent))
        FibonacciHeap<double, std::size_t> Q;
        for (std::size_t i = 0; i < n; i++) {
            if (i == r) {
                Q.Insert(0.0, i);
            } else {
                Q.Insert(std::numeric_limits<double>::max(), i);
            }
        }

        std::vector<std::pair<std::size_t, double>> infos (n, {-1, std::numeric_limits<double>::max()});
        while (!Q.isEmpty()) {
            auto up = Q.ExtractMin();
            std::size_t u = up->value;
            for (const auto& [v, w] : adj[u]) {
                auto vp = Q.SearchByValue(v);
                if (vp && w < vp->key) {
                    infos[v] = {u, w};
                    Q.DecreaseKey(vp, w);
                }
            }
        }
        for (std::size_t i = 0; i < n; i++) {
            if (i != r && infos[i].first != -1) {
                A.emplace_back(i, infos[i].first, infos[i].second);
            }
        }
        return A;
    }

    using EdgeMap = std::map<std::pair<std::size_t, std::size_t>, std::pair<std::size_t, std::size_t>>;

    std::tuple<Graph<n / 2>, EdgeMap, std::vector<Edge>> MSTReduce() {
        std::vector<int> mark (n);
        for (std::size_t v = 0; v < n; v++) {
            MakeSet(v);
        }
        std::vector<Edge> T;
        for (std::size_t u = 0; u < n; u++) {
            if (!mark[u]) {
                std::size_t v = -1;
                double w = std::numeric_limits<double>::max();
                for (const auto& [v_, w_] : adj[u]) {
                    if (w_ < w) {
                        v = v_;
                        w = w_;
                    }
                }
                Union(u, v);
                if (u < v) {
                    T.emplace_back(u, v, w);
                } else {
                    T.emplace_back(v, u, w);
                }
                mark[u] = 1;
                mark[v] = 1;
            }
        }
        Graph<n / 2> g2;
        std::size_t idx = 0;
        std::set<std::size_t> idxs;
        std::vector<std::size_t> idxmap (n);
        for (std::size_t i = 0; i < n; i++) {
            std::size_t si = FindSet(i);
            if (!idxs.contains(si)) {
                idxs.insert(si);
                idxmap[si] = idx;
                idx++;
            }
        }

        std::map<std::pair<std::size_t, std::size_t>, std::pair<std::size_t, std::size_t>> orig;

        for (std::size_t x = 0; x < n; x++) {
            for (const auto& [y, w] : adj[x]) {
                auto u = idxmap[FindSet(x)];
                auto v = idxmap[FindSet(y)];
                if (!g2.hasEdge(u, v)) {
                    g2.addEdge(u, v, w);
                    auto[u_, v_] = std::minmax(u, v);
                    auto[x_, y_] = std::minmax(x, y);
                    orig[{u_, v_}] = {x_, y_};
                } else if (w < g2.getEdgeWeight(u, v)) {
                    g2.setEdgeWeight(u, v, w);
                    auto[u_, v_] = std::minmax(u, v);
                    auto[x_, y_] = std::minmax(x, y);
                    orig[{u_, v_}] = {x_, y_};
                }
            }
        }
        return {g2, orig, T};
    }

    std::vector<Edge> MSTFast() {
        auto [g2, orig, T] = MSTReduce();
        auto A = g2.MSTPrim(0);
        for (const auto& [x, y, w] : A) {
            auto [x_, y_] = std::minmax(x, y);
            auto [u, v] = orig[{x_, y_}];
            T.emplace_back(u, v, w);
        }
        return T;
    }

};

int main() {
    constexpr std::size_t N = 3;
    Graph<12 * N - 3> g;
    for (std::size_t i = 0; i < 2 * N; i++) {
        g.addEdge(i, i + 1, i + 1.0);
        g.addEdge(i + 5 * N - 2, i + 5 * N - 1, 2.0 * i + 1.0);
        g.addEdge(i + 10 * N - 4, i + 10 * N - 3, 3.0 * i + 1.0);
    }
    g.addEdge(0, 2 * N + 1, 1.0);
    g.addEdge(N, 2 * N + 2, 1.0);
    g.addEdge(2 * N, 2 * N + 3, 1.0);
    g.addEdge(5 * N - 5, 5 * N - 2, 1.0);
    g.addEdge(5 * N - 4, 6 * N - 2, 1.0);
    g.addEdge(5 * N - 3, 7 * N - 2, 1.0);
    for (std::size_t i = 0; i < N - 2; i++) {
        g.addEdge(2 * N + 1 + 3 * i, 2 * N + 1 + 3 * (i + 1), 4.0 * i + 1.0);
        g.addEdge(2 * N + 2 + 3 * i, 2 * N + 2 + 3 * (i + 1), 5.0 * i + 1.0);
        g.addEdge(2 * N + 3 + 3 * i, 2 * N + 3 + 3 * (i + 1), 6.0 * i + 1.0);
        g.addEdge(7 * N - 1 + 3 * i, 7 * N - 1 + 3 * (i + 1), 7.0 * i + 1.0);
        g.addEdge(7 * N + 0 + 3 * i, 7 * N + 0 + 3 * (i + 1), 8.0 * i + 1.0);
        g.addEdge(7 * N + 1 + 3 * i, 7 * N + 1 + 3 * (i + 1), 9.0 * i + 1.0);
    }
    g.addEdge(5 * N - 2, 7 * N - 1, 1.0);
    g.addEdge(6 * N - 2, 7 * N + 0, 1.0);
    g.addEdge(7 * N - 2, 7 * N + 1, 1.0);
    g.addEdge(10 * N - 7, 10 * N - 4, 1.0);
    g.addEdge(10 * N - 6, 11 * N - 4, 1.0);
    g.addEdge(10 * N - 5, 12 * N - 4, 1.0);

    auto MST = g.MSTPrim(0);
    std::cout << "MST for g:\n";
    for (const auto& [src, dst, w] : MST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';

    auto MST2 = g.MSTFast();
    std::cout << "Fast MST for g:\n";
    for (const auto& [src, dst, w] : MST2) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
    std::cout << '\n';



}
