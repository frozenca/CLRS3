#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>
#include <tuple>
#include <concepts>
#include <limits>
#include <ranges>
#include <numeric>
#include <stack>
#include <memory>
#include <unordered_set>

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

template <std::size_t n, arithmetic T = double, bool undirected = false>
class Graph {
    std::vector<std::list<std::pair<std::size_t, T>>> adj;
    std::unordered_map<std::size_t, typename std::list<std::pair<std::size_t, T>>::iterator> edges;

public:
    Graph() : adj(n) {
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

    using Edge = std::tuple<std::size_t, std::size_t, T>;

    void InitializeSingleSource(FibonacciHeap<T, std::size_t>& Q, std::size_t s) {
        for (std::size_t i = 0; i < n; i++) {
            if (i == s) {
                Q.Insert(0.0, i);
            } else {
                Q.Insert(std::numeric_limits<double>::max(), i);
            }
        }
    }

    void Relax(FibonacciHeap<T, std::size_t>& Q, std::size_t u, T ud, std::size_t v,
               std::vector<std::size_t>& parents) {
        T w = getWeight(u, v);
        auto vp = Q.SearchByValue(v);
        if (vp && vp->key > ud + w) {
            Q.DecreaseKey(vp, ud + w);
            parents[v] = u;
        }
    }

    void Dijkstra(std::size_t s) {
        FibonacciHeap<T, std::size_t> Q;
        InitializeSingleSource(Q, s);

        std::vector<std::size_t> parents (n, -1);

        while (!Q.isEmpty()) {
            auto up = Q.ExtractMin();
            std::size_t u = up->value;
            T ud = up->key;
            for (const auto& [v, w] : adj[u]) {
                Relax(Q, u, ud, v, parents);
            }
        }

        for (std::size_t u = 0; u < n; u++) {
            printPath(s, u, parents);
            std::cout << '\n';
        }
    }

    void printPath(std::size_t source, std::size_t dest, std::vector<std::size_t>& parents) {
        if (dest == source) {
            std::cout << source << " ";
        } else if (parents[dest] == -1) {
            std::cout << "No path from " << source << " to " << dest << " exists\n";
        } else {
            printPath(source, parents[dest], parents);
            std::cout << dest << " ";
        }
    }
};

int main() {
    Graph<5> g;
    g.addEdge(0, 1, 10);
    g.addEdge(0, 3, 5);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 2);
    g.addEdge(2, 4, 4);
    g.addEdge(3, 1, 3);
    g.addEdge(3, 2, 9);
    g.addEdge(3, 4, 2);
    g.addEdge(4, 0, 7);
    g.addEdge(4, 2, 6);

    g.Dijkstra(0);


}