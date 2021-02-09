#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

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
        int p;
        int v;
        int m;
        Node* o;

        Node(const T& key, int p) : key {key}, parent {nullptr}, color {Color::Red}, p {p}, v {p}, m {p}, o {this} {}
    };

public:
    std::unique_ptr<Node> root;

private:
    void Validate(Node* x) {
        if (!x) return;

        x->v = (x->left ? x->left->v : 0) + x->p + (x->right ? x->right->v : 0);
        x->m = std::max({x->left ? x->left->m : 0, x->left ? x->left->v + x->p : x->p,
                (x->left ? x->left->v : 0) + (x->right ? x->right->m : 0) + x->p});
        if (x->left && x->m == x->left->m) {
            x->o = x->left.get();
        } else if (x->m == (x->left ? x->left->v + x->p : x->p)) {
            x->o = x;
        } else {
            x->o = x->right.get();
        }
    }


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
        Validate(px);
        Validate(py);
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
        Validate(px);
        Validate(py);
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

    void Insert(const T& a, const T& b) {
        assert(a <= b);
        auto z1 = std::make_unique<Node>(a, +1);
        Insert(std::move(z1));
        auto z2 = std::make_unique<Node>(b, -1);
        Insert(std::move(z2));
    }

    void Delete(const T& a, const T& b) {
        assert(a <= b);
        auto z1 = Search(root.get(), a);
        Delete(z1);
        auto z2 = Search(root.get(), b);
        Delete(z2);
    }

private:
    Node* Search(Node* x, const T& key) const {
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
            InsertFixup(root.get());
        } else if (z->key < y->key) {
            y->left = std::move(z);
            auto w = y;
            while (w) {
                Validate(w);
                w = w->parent;
            }
            InsertFixup(y->left.get());
        } else {
            y->right = std::move(z);
            auto w = y;
            while (w) {
                Validate(w);
                w = w->parent;
            }
            InsertFixup(y->right.get());
        }
    }

    void InsertFixup(Node* z) {
        while (z->parent && z->parent->color == Color::Red) {
            if (z->parent == z->parent->parent->left.get()) {
                auto y = z->parent->parent->right.get();
                if (y && y->color == Color::Red) {
                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right.get()) {
                        z = z->parent;
                        LeftRotate(z);
                    }
                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    RightRotate(z->parent->parent);
                }
            } else {
                auto y = z->parent->parent->left.get();
                if (y && y->color == Color::Red) {
                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left.get()) {
                        z = z->parent;
                        RightRotate(z);
                    }
                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    LeftRotate(z->parent->parent);
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

    Node* Minimum(Node* x) const {
        if (!x) {
            return x;
        }
        while (x->left) {
            x = x->left.get();
        }
        return x;
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
            auto w = xp;
            while (w) {
                Validate(w);
                w = w->parent;
            }
        } else if (!z->right) {
            x = z->left.get();
            xp = z->parent;
            auto pz = Transplant(z, std::move(z->left));
            auto upz = std::unique_ptr<Node>(pz);
            auto w = xp;
            while (w) {
                Validate(w);
                w = w->parent;
            }
        } else {
            auto y = Minimum(z->right.get());
            orig_color = y->color;
            x = y->right.get();
            if (y->parent == z) {
                if (x) {
                    x->parent = y;
                }
                xp = y;
                auto pz = Transplant(z, std::move(z->right));
                y->left = std::move(pz->left);
                y->left->parent = y;
                y->color = pz->color;
                auto upz = std::unique_ptr<Node>(pz);
                auto w = xp;
                while (w) {
                    Validate(w);
                    w = w->parent;
                }
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
                auto w = xp;
                while (w) {
                    Validate(w);
                    w = w->parent;
                }
            }
        }
        if (orig_color == Color::Black) {
            DeleteFixup(x, xp);
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
        os << std::setprecision(3) << tree.root.get();
        return os;
    }
};

int main() {
    constexpr size_t SIZE = 100;
    std::uniform_real_distribution<> dist(0.0, 1.0);
    RBTree<double> rbtree;
    for (size_t i = 0; i < SIZE; i++) {
        auto x_begin = dist(gen);
        auto x_end = dist(gen);
        if (x_begin > x_end) std::swap(x_begin, x_end);
        rbtree.Insert(x_begin, x_end);
        std::cout << rbtree << '\n';
        std::cout << rbtree.root->key << '(' << rbtree.root->o->key << ")\n";
    }

}