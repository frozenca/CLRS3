#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

template <typename T>
struct RBTree {
private:
    enum class Color {
        Red,
        Black
    };

    struct Node {
        T key;
        Color color;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node* parent;
        Node* succ;
        Node* pred;

        Node(const T& key) : key {key}, color {Color::Red}, parent {nullptr}, succ {nullptr}, pred {nullptr} {}
    };

public:
    std::unique_ptr<Node> root;
    T minimum = std::numeric_limits<T>::max();
    T maximum = std::numeric_limits<T>::lowest();

private:
    void LeftRotate(std::unique_ptr<Node>&& x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        if (x->right) {
            x->right->parent = x.get();
        }
        y->parent = x->parent;
        auto xp = x->parent;
        if (!xp) {
            auto px = x.release();
            root = std::move(y);
            root->left = std::unique_ptr<Node>(px);
            root->left->parent = root.get();
        } else if (x == xp->left) {
            auto px = x.release();
            xp->left = std::move(y);
            xp->left->left = std::unique_ptr<Node>(px);
            xp->left->left->parent = xp->left.get();
        } else {
            auto px = x.release();
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
        if (!xp) {
            auto px = x.release();
            root = std::move(y);
            root->right = std::unique_ptr<Node>(px);
            root->right->parent = root.get();
        } else if (x == xp->left) {
            auto px = x.release();
            xp->left = std::move(y);
            xp->left->right = std::unique_ptr<Node>(px);
            xp->left->right->parent = xp->left.get();
        } else {
            auto px = x.release();
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
    [[nodiscard]] bool Search(const T& key) const {
        return Search(root.get(), key);
    }

    void Insert(const T& key) {
        auto z = std::make_unique<Node>(key);
        Insert(std::move(z));
    }

    void Delete(const T& key) {
        auto z = Search(root.get(), key);
        Delete(z);
    }

private:
    [[nodiscard]] Node* Minimum(Node* x) const {
        if (!x) {
            return x;
        }
        while (x->left) {
            x = x->left.get();
        }
        return x;
    }

    [[nodiscard]] Node* Maximum(Node* x) const {
        if (!x) {
            return x;
        }
        while (x->right) {
            x = x->right.get();
        }
        return x;
    }

    [[nodiscard]] Node* Successor(const Node* x) const {
        if (!x) return nullptr;
        if (x->right) {
            return Minimum(x->right.get());
        }
        auto y = x->parent;
        while (y && x == y->right.get()) {
            x = y;
            y = y->parent;
        }
        return y;
    }

    [[nodiscard]] Node* Predecessor(const Node* x) const {
        if (!x) return nullptr;
        if (x->left) {
            return Maximum(x->left.get());
        }
        auto y = x->parent;
        while (y && x == y->left.get()) {
            x = y;
            y = y->parent;
        }
        return y;
    }

    [[nodiscard]] Node* Search(Node* x, const T& key) const {
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
        Node* pz = z.get();
        if (!y) {
            root = std::move(z);
            InsertFixup(std::move(root));
        } else if (z->key < y->key) {
            y->left = std::move(z);
            InsertFixup(std::move(y->left));
        } else {
            y->right = std::move(z);
            InsertFixup(std::move(y->right));
        }
        pz->pred = Predecessor(pz);
        pz->succ = Predecessor(pz);
        maximum = std::max(pz->key, maximum);
        minimum = std::max(pz->key, minimum);
    }

    void InsertFixup(std::unique_ptr<Node>&& z) {
        auto zp = z->parent;
        while (zp && zp->color == Color::Red) {
            auto zpp = zp->parent;
            if (zp == zpp->left.get()) {
                auto y = zpp->right.get();
                if (y && y->color == Color::Red) {
                    zp->color = Color::Black;
                    y->color = Color::Black;
                    zpp->color = Color::Red;
                    zp = zpp->parent;
                } else {
                    if (z == zp->right) {
                        LeftRotate(std::move(zpp->left));
                        zp = zpp->left.get();
                    }
                    zp->color = Color::Black;
                    zpp->color = Color::Red;
                    RightRotate(zpp);
                }
            } else {
                auto y = zpp->left.get();
                if (y && y->color == Color::Red) {
                    zp->color = Color::Black;
                    y->color = Color::Black;
                    zpp->color = Color::Red;
                    zp = zpp->parent;
                } else {
                    if (z == zp->left) {
                        RightRotate(std::move(zpp->right));
                        zp = zpp->right.get();
                    }
                    zp->color = Color::Black;
                    zpp->color = Color::Red;
                    LeftRotate(zpp);
                }
            }
        }
        root->color = Color::Black;
    }

    Node* Transplant(Node* u, std::unique_ptr<Node>&& v) {
        if (v) {
            v->parent = u->parent;
        }
        Node* w = nullptr;
        if (!u->parent) {
            w = root.release();
            root = std::move(v);
        } else if (u == u->parent->left.get()) {
            w = u->parent->left.release();
            u->parent->left = std::move(v);
        } else {
            w = u->parent->right.release();
            u->parent->right = std::move(v);
        }
        return w;
    }

    void Delete(Node* z) {
        if (!z) {
            return;
        }
        Color orig_color = z->color;
        Node* x = nullptr;
        Node* xp = nullptr;
        if (!z->left) {
            x = z->right.get();
            xp = z->parent;
            auto pz = Transplant(z, std::move(z->right));
            auto upz = std::unique_ptr<Node>(pz);
        } else if (!z->right) {
            x = z->left.get();
            xp = z->parent;
            auto pz = Transplant(z, std::move(z->left));
            auto upz = std::unique_ptr<Node>(pz);
        } else {
            auto y = Minimum(z->right.get());
            orig_color = y->color;
            x = y->right.get();
            xp = y;
            if (y->parent == z) {
                if (x) {
                    x->parent = y;
                }
                auto pz = Transplant(z, std::move(z->right));
                y->left = std::move(pz->left);
                y->left->parent = y;
                y->color = pz->color;
                auto upz = std::unique_ptr<Node>(pz);
            } else {
                xp = y->parent;
                auto py = Transplant(y, std::move(y->right));
                py->right = std::move(z->right);
                py->right->parent = py;
                auto upy = std::unique_ptr<Node>(py);
                auto pz = Transplant(z, std::move(upy));
                py->left = std::move(pz->left);
                py->left->parent = py;
                py->color = pz->color;
                auto upz = std::unique_ptr<Node>(pz);
            }
        }
        if (orig_color == Color::Black) {
            DeleteFixup(x, xp);
        }
        if (z->succ) {
            z->succ->pred = z->pred;
        }
        if (z->pred) {
            z->pred->succ = z->succ;
        }
        if (z->key == minimum) {
            minimum = z->succ ? z->succ->key : std::numeric_limits<T>::max();
        }
        if (z->key == maximum) {
            maximum = z->pred ? z->pred->key : std::numeric_limits<T>::lowest();
        }
    }

    void DeleteFixup(Node* x, Node* xp) {
        while (x != root.get() && (!x || x->color == Color::Black)) {
            if (x == xp->left.get()) {
                Node* w = xp->right.get();
                if (w && w->color == Color::Red) {
                    w->color = Color::Black;
                    xp->color = Color::Red;
                    LeftRotate(xp);
                    w = xp->right.get();
                }
                if (w && (!w->left || w->left->color == Color::Black)
                    && (!w->right || w->right->color == Color::Black)) {
                    w->color = Color::Red;
                    x = xp;
                    xp = xp->parent;
                } else if (w) {
                    if (!w->right || w->right->color == Color::Black) {
                        w->left->color = Color::Black;
                        w->color = Color::Red;
                        RightRotate(w);
                        w = xp->right.get();
                    }
                    w->color = xp->color;
                    xp->color = Color::Black;
                    w->right->color = Color::Black;
                    LeftRotate(xp);
                    x = root.get();
                } else {
                    x = root.get();
                }
            } else {
                Node* w = xp->left.get();
                if (w && w->color == Color::Red) {
                    w->color = Color::Black;
                    xp->color = Color::Red;
                    RightRotate(xp);
                    w = xp->left.get();
                }
                if (w && (!w->left || w->left->color == Color::Black)
                    && (!w->right || w->right->color == Color::Black)) {
                    w->color = Color::Red;
                    x = xp;
                    xp = xp->parent;
                } else if (w) {
                    if (!w->left || w->left->color == Color::Black) {
                        w->right->color = Color::Black;
                        w->color = Color::Red;
                        LeftRotate(w);
                        w = xp->left.get();
                    }
                    w->color = xp->color;
                    xp->color = Color::Black;
                    w->left->color = Color::Black;
                    RightRotate(xp);
                    x = root.get();
                } else {
                    x = root.get();
                }
            }
        }
        if (x) {
            x->color = Color::Black;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, Node* node) {
        if (node) {
            os << node->left.get();
            os << node->key;
            if (node->color == Color::Black) {
                os << "● ";
            } else {
                os << "○ ";
            }
            os << node->right.get();
        }
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const RBTree& tree) {
        os << tree.root.get();
        return os;
    }
};

int main() {
    constexpr size_t SIZE = 100;
    std::vector<int> v (SIZE);
    std::iota(v.begin(), v.end(), 1);
    sr::shuffle(v, gen);
    RBTree<int> rbtree;
    for (auto n : v) {
        rbtree.Insert(n);
    }
    std::cout << '\n';
    std::cout << rbtree << '\n';
    sr::shuffle(v, gen);
    for (auto n : v) {
        rbtree.Delete(n);
        std::cout << rbtree << '\n';
    }

}