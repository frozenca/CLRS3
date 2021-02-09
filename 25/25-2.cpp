#include <array>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <concepts>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <queue>
#include <random>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace sr = std::ranges;
namespace crn = std::chrono;

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

constexpr std::size_t V = 400;
std::mt19937 gen(std::random_device{}());
constexpr double dens = 0.2;
std::size_t D = std::floor(std::pow(V, 1 + std::log(dens)/std::log(V)));
std::bernoulli_distribution dist(dens);
std::uniform_real_distribution<> w_dist(1, 100);

size_t parent(size_t i) {
    if (i == 0) return 0;
    return (i - 1) / D;
}

size_t dAryChild(size_t i, size_t j) {
    return D * i + j + 1;
}

template <typename T>
void minHeapify(std::pair<std::vector<T>&, size_t>& A, size_t i) {
    size_t smallest = i;
    for (size_t j = 0; j < D; j++) {
        size_t child = dAryChild(i, j);
        if (child < A.second && A.first[child] < A.first[smallest]) {
            smallest = child;
        }
    }
    if (smallest != i) {
        std::swap(A.first[i], A.first[smallest]);
        minHeapify(A, smallest);
    }
}

template <typename T>
void buildMinHeap(std::pair<std::vector<T>&, size_t>& A) {
    A.second = A.first.size();
    for (size_t i = A.first.size() / D; i < A.first.size(); i--) {
        minHeapify(A, i);
    }
}

template <typename T>
T heapExtractMin(std::pair<std::vector<T>&, size_t>& A) {
    if (A.second < 1) {
        throw std::underflow_error("heap underflow");
    }
    T minElem = A.first[0];
    A.first[0] = A.first[A.second - 1];
    A.second--;
    minHeapify(A, 0);
    return minElem;
}

template <typename T>
void heapDecreaseKey(std::pair<std::vector<T>&, size_t>& A, size_t i, const T& key) {
    if (key < A.first[i]) {
        throw std::runtime_error("new key is smaller than current key");
    }
    A.first[i] = key;
    while (i > 0 && A.first[parent(i)] > A.first[i]) {
        std::swap(A.first[i], A.first[parent(i)]);
        i = parent(i);
    }
}

template <typename T>
void minHeapInsert(std::pair<std::vector<T>&, size_t>& A, const T& key) {
    A.second++;
    if (A.first.size() < A.second) {
        A.first.push_back(std::numeric_limits<T>::max());
    } else {
        A.first[A.second - 1] = std::numeric_limits<T>::max();
    }
    heapDecreaseKey(A, A.second - 1, key);
}



template <std::size_t n, arithmetic T = double, bool undirected = false>
class GraphM final {
    std::array<T, n * n> W;

public:
    GraphM() {
        sr::fill(W, std::numeric_limits<T>::max());
        for (std::size_t i = 0; i < n; i++) {
            W[i * n + i] = T{0};
        }
    }

    void addEdge(std::size_t src, std::size_t dst, T weight) {
        assert(src < n && dst < n);
        W[src * n + dst] = weight;
        if (undirected) {
            W[dst * n + src] = weight;
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        addEdge(src, dst, std::numeric_limits<T>::max());
    }

    [[nodiscard]] T getWeight(std::size_t src, std::size_t dst) const {
        return W[src * n + dst];
    }

    [[nodiscard]] std::array<T, n * n> FloydWarshall() const {
        auto D = W;
        for (std::size_t k = 0; k < n; k++) {
            std::array<T, n * n> D_;
            for (std::size_t i = 0; i < n; i++) {
                for (std::size_t j = 0; j < n; j++) {
                    D_[i * n + j] = std::min(D[i * n + j], D[i * n + k] + D[k * n + j]);
                }
            }
            std::swap(D, D_);
        }
        return D;
    }
};

template <typename T>
concept hashable = requires (T a) {
    {std::hash<T>{}(a)} -> std::convertible_to<std::size_t>;
};

template <arithmetic T, hashable V>
class FibonacciHeap final {
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

template <std::size_t n, arithmetic T = double, bool undirected = false>
class Graph final {
    std::vector<std::list<std::pair<std::size_t, T>>> adj;
    std::unordered_map<std::size_t, typename std::list<std::pair<std::size_t, T>>::iterator> edges;

public:
    Graph() : adj(n) {
    }

    Graph(const Graph& g) : adj(n) {
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : g.adj) {
                addEdge(u, v, w);
            }
        }
    }

    Graph& operator=(const Graph& g) {
        if (&g == this) {
            return *this;
        }
        adj.clear();
        adj.resize(n);
        edges.clear();
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : g.adj) {
                addEdge(u, v, w);
            }
        }
    }

    Graph(Graph&& g) noexcept : adj(std::move(g.adj)), edges(std::move(g.edges)) {}

    Graph& operator=(Graph&& g) noexcept {
        adj = std::move(g.adj);
        edges = std::move(g.edges);
        return *this;
    }

    void addEdge(std::size_t src, std::size_t dst, T weight) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, weight);
            edges[src * n + dst] = adj[src].begin();
        }
        if (undirected) {
            if (!edges.contains(dst * n + src)) {
                adj[dst].emplace_front(src, weight);
                edges[dst * n + src] = adj[dst].begin();
            }
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        if (undirected) {
            adj[dst].erase(edges[dst * n + src]);
            edges.erase(dst * n + src);
        }
    }

    [[nodiscard]] T getWeight(std::size_t src, std::size_t dst) const {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            return T {0};
        } else {
            return edges.at(src * n + dst)->second;
        }
    }

    void setWeight(std::size_t src, std::size_t dst, T weight) {
        assert(src < n && dst < n);
        removeEdge(src, dst);
        addEdge(src, dst, weight);
    }

    using Edge = std::tuple<std::size_t, std::size_t, T>;

    struct PathInfo {
        T d = std::numeric_limits<T>::max();
        std::size_t parent = -1;
    };

    void InitializeSingleSource(std::vector<PathInfo>& pinfos, std::size_t s) const {
        pinfos.resize(0);
        pinfos.resize(n);
        pinfos[s].d = T {0};
    }

    void Relax(std::vector<PathInfo>& pinfos, std::size_t u, std::size_t v) const {
        T w = getWeight(u, v);
        if (pinfos[v].d > pinfos[u].d + w) {
            pinfos[v].d = pinfos[u].d + w;
            pinfos[v].parent = u;
        }
    }

    [[nodiscard]] std::pair<bool, std::vector<PathInfo>> BellmanFord(std::size_t s) const {
        std::vector<PathInfo> pinfos;
        InitializeSingleSource(pinfos, s);
        for (std::size_t i = 0; i < n - 1; i++) {
            for (std::size_t u = 0; u < n; u++) {
                for (const auto& [v, w] : adj[u]) {
                    Relax(pinfos, u, v);
                }
            }
        }
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                if (pinfos[v].d > pinfos[u].d + w) {
                    return {false, {}};
                }
            }
        }
        return {true, pinfos};
    }

    void InitializeSingleSource(FibonacciHeap<T, std::size_t>& Q, std::size_t s) const {
        for (std::size_t i = 0; i < n; i++) {
            if (i == s) {
                Q.Insert(0.0, i);
            } else {
                Q.Insert(std::numeric_limits<T>::max(), i);
            }
        }
    }

    void Relax(FibonacciHeap<T, std::size_t>& Q, std::size_t u, T ud, std::size_t v,
               std::vector<std::size_t>& parents, std::vector<T>& dists) const {
        T w = getWeight(u, v);
        auto vp = Q.SearchByValue(v);
        if (vp && vp->key > ud + w) {
            Q.DecreaseKey(vp, ud + w);
            parents[v] = u;
            dists[v] = ud + w;
        }
    }

    [[nodiscard]] std::vector<T> Dijkstra(std::size_t s) const {
        FibonacciHeap<T, std::size_t> Q;
        InitializeSingleSource(Q, s);

        std::vector<std::size_t> parents (n, -1);
        std::vector<T> dists (n, std::numeric_limits<T>::max());
        dists[s] = T{0};

        while (!Q.isEmpty()) {
            auto up = Q.ExtractMin();
            std::size_t u = up->value;
            T ud = up->key;
            for (const auto& [v, w] : adj[u]) {
                Relax(Q, u, ud, v, parents, dists);
            }
        }
        return dists;
    }

    [[nodiscard]] std::vector<T> DijkstraPQ(std::size_t s) const {
        std::priority_queue<std::pair<T, std::size_t>, std::vector<std::pair<T, std::size_t>>, std::greater<>> Q;
        Q.emplace(0.0, s);

        std::vector<T> dists (n, std::numeric_limits<T>::max());
        dists[s] = T{0};

        while (!Q.empty()) {
            auto [d, u] = Q.top();
            Q.pop();
            for (const auto& [v, w_] : adj[u]) {
                if (dists[v] > d + w_) {
                    dists[v] = d + w_;
                    Q.emplace(dists[v], v);
                }
            }
        }
        return dists;
    }

    [[nodiscard]] std::vector<T> DijkstraDaryHeap(std::size_t s) const {
        std::vector<T> dists (n, std::numeric_limits<T>::max());
        dists[s] = T{0};
        std::vector<std::pair<T, std::size_t>> A; // d-ary min heap
        for (std::size_t i = 0; i < n; i++) {
            if (i == s) {
                A.emplace_back(T{0}, s);
            } else {
                A.emplace_back(std::numeric_limits<T>::max(), i);
            }
        }
        std::pair<std::vector<std::pair<T, std::size_t>>&, std::size_t> Q = {A, A.size()};
        buildMinHeap(Q);

        while (Q.second > 0) {
            auto [d, u] = heapExtractMin(Q);
            for (const auto& [v, w_] : adj[u]) {
                if (dists[v] > d + w_) {
                    dists[v] = d + w_;
                    minHeapInsert(Q, {dists[v], v});
                }
            }
        }

        return dists;
    }

    [[nodiscard]] std::array<T, n * n> Johnson() const {
        Graph<n + 1> G_;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                G_.addEdge(u, v, w);
            }
            G_.addEdge(n, u, T{0});
        }
        auto [no_negative_cycle, pinfos] = G_.BellmanFord(n);
        if (!no_negative_cycle) {
            std::cout << "The input graph contains a negative weight cycle\n";
            return {};
        }
        Graph G_reweighted;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                G_reweighted.addEdge(u, v, w + pinfos[u].d - pinfos[v].d);
            }
        }
        std::array<T, n * n> D;
        for (std::size_t u = 0; u < n; u++) {
            auto dists = G_reweighted.Dijkstra(u);
            for (std::size_t v = 0; v < n; v++) {
                D[u * n + v] = dists[v] + pinfos[v].d - pinfos[u].d;
            }
        }
        return D;
    }

    [[nodiscard]] std::array<T, n * n> JohnsonPQ() const {
        Graph<n + 1> G_;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                G_.addEdge(u, v, w);
            }
            G_.addEdge(n, u, T{0});
        }
        auto [no_negative_cycle, pinfos] = G_.BellmanFord(n);
        if (!no_negative_cycle) {
            std::cout << "The input graph contains a negative weight cycle\n";
            return {};
        }
        Graph G_reweighted;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto& [v, w] : adj[u]) {
                G_reweighted.addEdge(u, v, w + pinfos[u].d - pinfos[v].d);
            }
        }
        std::array<T, n * n> D;
        for (std::size_t u = 0; u < n; u++) {
            auto dists = G_reweighted.DijkstraPQ(u);
            for (std::size_t v = 0; v < n; v++) {
                D[u * n + v] = dists[v] + pinfos[v].d - pinfos[u].d;
            }
        }
        return D;
    }

    [[nodiscard]] std::array<T, n * n> DijkstraAll() const {
        std::array<T, n * n> D;
        for (std::size_t u = 0; u < n; u++) {
            auto dists = Dijkstra(u);
            for (std::size_t v = 0; v < n; v++) {
                D[u * n + v] = dists[v];
            }
        }
        return D;
    }

    [[nodiscard]] std::array<T, n * n> DijkstraPQAll() const {
        std::array<T, n * n> D;
        for (std::size_t u = 0; u < n; u++) {
            auto dists = DijkstraPQ(u);
            for (std::size_t v = 0; v < n; v++) {
                D[u * n + v] = dists[v];
            }
        }
        return D;
    }

    [[nodiscard]] std::array<T, n * n> DijkstraDaryHeapAll() const {
        std::array<T, n * n> D;
        for (std::size_t u = 0; u < n; u++) {
            auto dists = DijkstraDaryHeap(u);
            for (std::size_t v = 0; v < n; v++) {
                D[u * n + v] = dists[v];
            }
        }
        return D;
    }

};

template <std::size_t N>
[[nodiscard]] bool allClose(const std::array<double, N>& A1,
                            const std::array<double, N>& A2) {
    assert(A1.size() == A2.size());
    return sr::mismatch(A1, A2, [](auto d1, auto d2) {return std::abs(d1 - d2) < 1e-4;}).in1 == A1.end();
}

int main() {
    Graph<V> g1;
    GraphM<V> g2;
    for (std::size_t i = 0; i < V; i++) {
        for (std::size_t j = i + 1; j < V; j++) {
            if (dist(gen)) {
                auto w = w_dist(gen);
                g1.addEdge(i, j, w);
                g2.addEdge(i, j, w);
            }
            if (dist(gen)) {
                auto w = w_dist(gen);
                g1.addEdge(j, i, w);
                g2.addEdge(j, i, w);
            }
        }
    }

    auto t1 = crn::steady_clock::now();
    auto D1 = g1.Johnson();
    auto t2 = crn::steady_clock::now();
    auto dt1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << "Johnson with Fibonacci Heap : " << dt1.count() << "ms\n";
    auto t3 = crn::steady_clock::now();
    auto D2 = g2.FloydWarshall();
    auto t4 = crn::steady_clock::now();
    auto dt2 = crn::duration_cast<crn::milliseconds>(t4 - t3);
    std::cout << "Floyd-Warshall : " << dt2.count() << "ms\n";
    auto t5 = crn::steady_clock::now();
    auto D3 = g1.JohnsonPQ();
    auto t6 = crn::steady_clock::now();
    auto dt3 = crn::duration_cast<crn::milliseconds>(t6 - t5);
    std::cout << "Johnson with std::priority_queue : " << dt3.count() << "ms\n";
    auto t7 = crn::steady_clock::now();
    auto D4 = g1.DijkstraPQAll();
    auto t8 = crn::steady_clock::now();
    auto dt4 = crn::duration_cast<crn::milliseconds>(t8 - t7);
    std::cout << "Dijkstra V times with std::priority_queue : " << dt4.count() << "ms\n";
    auto t9 = crn::steady_clock::now();
    auto D5 = g1.DijkstraAll();
    auto t10 = crn::steady_clock::now();
    auto dt5 = crn::duration_cast<crn::milliseconds>(t10 - t9);
    std::cout << "Dijkstra V times with Fibonacci Heap : " << dt5.count() << "ms\n";
    auto t11 = crn::steady_clock::now();
    auto D6 = g1.DijkstraDaryHeapAll();
    auto t12 = crn::steady_clock::now();
    auto dt6 = crn::duration_cast<crn::milliseconds>(t12 - t11);
    std::cout << "Dijkstra V times with " << D << "-ary heap : " << dt6.count() << "ms\n";
    assert(allClose(D1, D2) && allClose(D2, D3) && allClose(D3, D4)
    && allClose(D4, D5) && allClose(D5, D6));

}