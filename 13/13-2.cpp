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
struct AVLTree {
private:
    struct Node {
        T key;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node* parent;
        int balance;

        Node(const T& key) : key {key}, parent {nullptr}, balance {0} {}
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
        Node* py = nullptr;
        if (!xp) {
            px = x.release();
            root = std::move(y);
            root->left = std::unique_ptr<Node>(px);
            root->left->parent = root.get();
            py = root.get();
        } else if (x == xp->left) {
            px = x.release();
            xp->left = std::move(y);
            xp->left->left = std::unique_ptr<Node>(px);
            xp->left->left->parent = xp->left.get();
            py = xp->left.get();
        } else {
            px = x.release();
            xp->right = std::move(y);
            xp->right->left = std::unique_ptr<Node>(px);
            xp->right->left->parent = xp->right.get();
            py = xp->right.get();
        }

        if (py->balance == 0) {
            px->balance++;
            py->balance = px->balance + 1;
        } else if (py->balance == 1) {
            px->balance++;
            py->balance = px->balance + 2;
        } else if (py->balance == -1) {
            px->balance += 2;
            py->balance = px->balance;
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
        Node* py = nullptr;
        if (!xp) {
            px = x.release();
            root = std::move(y);
            root->right = std::unique_ptr<Node>(px);
            root->right->parent = root.get();
            py = root.get();
        } else if (x == xp->left) {
            px = x.release();
            xp->left = std::move(y);
            xp->left->right = std::unique_ptr<Node>(px);
            xp->left->right->parent = xp->left.get();
            py = xp->left.get();
        } else {
            px = x.release();
            xp->right = std::move(y);
            xp->right->right = std::unique_ptr<Node>(px);
            xp->right->right->parent = xp->right.get();
            py = xp->right.get();
        }

        if (py->balance == 0) {
            px->balance--;
            py->balance = px->balance - 1;
        } else if (py->balance == -1) {
            px->balance--;
            py->balance = px->balance - 2;
        } else if (py->balance == 1) {
            px->balance -= 2;
            py->balance = px->balance;
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

    void Insert(const T& key) {
        auto z = std::make_unique<Node>(key);
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
            y->balance++;
            auto yp = y->parent;
            if (yp) {
                yp->balance++;
            }
            if (yp && y == yp->left.get()) { // left left
                if (yp->balance > 1) {
                    RightRotate(yp);
                }
            } else if (yp && y == yp->right.get()) { // right left
                if (y->balance >= 1) {
                    RightRotate(y);
                }
                if (yp->balance < -1) {
                    LeftRotate(yp);
                }
            }
        } else {
            y->right = std::move(z);
            y->balance--;
            auto yp = y->parent;
            if (yp) {
                yp->balance--;
            }
            if (yp && y == yp->right.get()) { // right right
                if (yp->balance < -1) {
                    LeftRotate(yp);
                }
            } else if (yp && y == yp->left.get()) { // left right
                if (y->balance <= -1) {
                    LeftRotate(y);
                }
                if (yp->balance > 1) {
                    RightRotate(yp);
                }
            }
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

    friend std::ostream& operator<<(std::ostream& os, const AVLTree& tree) {
        os << tree.root.get();
        return os;
    }
};

int main() {
    constexpr size_t SIZE = 100;
    std::vector<int> v (SIZE);
    std::iota(v.begin(), v.end(), 1);
    sr::shuffle(v, gen);
    AVLTree<int> avltree;
    for (auto n : v) {
        avltree.Insert(n);
        std::cout << avltree << '\n';
    }

}