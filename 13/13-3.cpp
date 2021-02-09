#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

template <typename T>
struct Treap {
private:
    struct Node {
        T key;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node* parent;
        int priority;

        Node(const T& key, int priority) : key {key}, parent {nullptr}, priority {priority} {}
    };

public:
    std::unique_ptr<Node> root;

private:
    void LeftRotate(std::unique_ptr<Node>&& x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        if (x->right) {
            x->right->parent = x.get();
        }
        y->parent = x->parent;
        auto xp = x->parent;
        Node* px = nullptr;
        if (!xp) {
            px = x.release();
            root = std::move(y);
            root->left = std::unique_ptr<Node>(px);
            root->left->parent = root.get();
        } else if (x == xp->left) {
            px = x.release();
            xp->left = std::move(y);
            xp->left->left = std::unique_ptr<Node>(px);
            xp->left->left->parent = xp->left.get();
        } else {
            px = x.release();
            xp->right = std::move(y);
            xp->right->left = std::unique_ptr<Node>(px);
            xp->right->left->parent = xp->right.get();
        }

    }

    void RightRotate(std::unique_ptr<Node>&& x) {
        auto y = std::move(x->left);
        x->left = std::move(y->right);
        if (x->left) {
            x->left->parent = x.get();
        }
        y->parent = x->parent;
        auto xp = x->parent;
        Node* px = nullptr;
        if (!xp) {
            px = x.release();
            root = std::move(y);
            root->right = std::unique_ptr<Node>(px);
            root->right->parent = root.get();
        } else if (x == xp->left) {
            px = x.release();
            xp->left = std::move(y);
            xp->left->right = std::unique_ptr<Node>(px);
            xp->left->right->parent = xp->left.get();
        } else {
            px = x.release();
            xp->right = std::move(y);
            xp->right->right = std::unique_ptr<Node>(px);
            xp->right->right->parent = xp->right.get();
        }

    }

    void LeftRotate(Node* x) {
        auto xp = x->parent;
        if (!xp) {
            LeftRotate(std::move(root));
        } else if (x == xp->left.get()) {
            LeftRotate(std::move(xp->left));
        } else {
            LeftRotate(std::move(xp->right));
        }
    }

    void RightRotate(Node* x) {
        auto xp = x->parent;
        if (!xp) {
            RightRotate(std::move(root));
        } else if (x == xp->left.get()) {
            RightRotate(std::move(xp->left));
        } else {
            RightRotate(std::move(xp->right));
        }
    }

public:
    bool Search(const T& key) {
        return Search(root.get(), key);
    }

    void Insert(const T& key, int priority) {
        auto z = std::make_unique<Node>(key, priority);
        Insert(std::move(z));
    }

private:
    Node* Search(Node* x, const T& key) {
        if (!x || x->key == key) {
            return x;
        }
        if (key < x->key) {
            return Search(x->left.get(), key);
        } else {
            return Search(x->right.get(), key);
        }
    }

    void Insert(std::unique_ptr<Node> z) {
        Node* y = nullptr;
        Node* x = root.get();
        while (x) {
            y = x;
            if (z->key < x->key) {
                x = x->left.get();
            } else {
                x = x->right.get();
            }
        }
        z->parent = y;
        if (!y) {
            root = std::move(z);
        } else if (z->key < y->key) {
            y->left = std::move(z);
        } else {
            y->right = std::move(z);
        }

        while (y) {
            auto yp = y->parent;
            if (y->left && y->left->priority > y->priority) {
                RightRotate(y);
            } else if (y->right && y->right->priority > y->priority) {
                LeftRotate(y);
            }
            y = yp;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, Node* node) {
        if (node) {
            os << node->left.get();
            os << node->key << ' ';
            os << node->right.get();
        }
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const Treap& tree) {
        os << tree.root.get();
        return os;
    }
};

int main() {
    constexpr size_t SIZE = 100;
    std::vector<int> v (SIZE);
    std::iota(v.begin(), v.end(), 1);
    sr::shuffle(v, gen);
    Treap<int> treap;
    auto u = v;
    sr::shuffle(u, gen);
    for (size_t i = 0; i < v.size(); i++) {
        treap.Insert(v[i], u[i]);
        std::cout << treap << '\n';
    }

}