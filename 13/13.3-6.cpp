#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
#include <numeric>
#include <vector>
#include <random>
#include <stack>
#include <algorithm>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

enum class Color {
    Red,
    Black
};

template <typename T>
struct Node {
    T key;
    Color color;
    std::unique_ptr<Node<T>> left;
    std::unique_ptr<Node<T>> right;

    Node(const T& key) : key {key}, color {Color::Red} {}
};

template <typename T>
struct RBTree {
public:
    std::unique_ptr<Node<T>> root;

private:
    void LeftRotate(Node<T>* xp, std::unique_ptr<Node<T>>&& x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        if (!xp) {
            auto px = x.release();
            root = std::move(y);
            root->left = std::unique_ptr<Node<T>>(px);
        } else if (x == xp->left) {
            auto px = x.release();
            xp->left = std::move(y);
            xp->left->left = std::unique_ptr<Node<T>>(px);
        } else {
            auto px = x.release();
            xp->right = std::move(y);
            xp->right->left = std::unique_ptr<Node<T>>(px);
        }
    }

    void RightRotate(Node<T>* xp, std::unique_ptr<Node<T>>&& x) {
        auto y = std::move(x->left);
        x->left = std::move(y->right);
        if (!xp) {
            auto px = x.release();
            root = std::move(y);
            root->right = std::unique_ptr<Node<T>>(px);
        } else if (x == xp->left) {
            auto px = x.release();
            xp->left = std::move(y);
            xp->left->right = std::unique_ptr<Node<T>>(px);
        } else {
            auto px = x.release();
            xp->right = std::move(y);
            xp->right->right = std::unique_ptr<Node<T>>(px);
        }
    }

public:
    Node<T>* Search(const T& key) {
        return Search(root.get(), key);
    }

    void Insert(const T& key) {
        auto z = std::make_unique<Node<T>>(key);
        Insert(std::move(z));
    }

private:
    Node<T>* Search(Node<T>* x, const T& key) {
        if (!x || x->key == key) {
            return x;
        }
        if (key < x->key) {
            return Search(x->left.get(), key);
        } else {
            return Search(x->right.get(), key);
        }
    }

    void Insert(std::unique_ptr<Node<T>> z) {
        std::stack<Node<T>*> s;
        Node<T>* y = nullptr;
        Node<T>* x = root.get();
        while (x) {
            y = x;
            s.push(x);
            if (z->key < x->key) {
                x = x->left.get();
            } else {
                x = x->right.get();
            }
        }
        if (!y) {
            root = std::move(z);
            InsertFixup(s, std::move(root));
        } else if (z->key < y->key) {
            y->left = std::move(z);
            InsertFixup(s, std::move(y->left));
        } else {
            y->right = std::move(z);
            InsertFixup(s, std::move(y->right));
        }
    }

    void InsertFixup(std::stack<Node<T>*>& s, std::unique_ptr<Node<T>>&& z) {
        Node<T>* zp = nullptr;
        if (!s.empty()) {
            zp = s.top();
        }
        while (zp && zp->color == Color::Red) {
            zp = s.top();
            s.pop();
            auto zpp = s.top();
            if (zp == zpp->left.get()) {
                auto y = zpp->right.get();
                if (y && y->color == Color::Red) {
                    zp->color = Color::Black;
                    y->color = Color::Black;
                    zpp->color = Color::Red;
                    s.pop();
                } else {
                    if (z == zp->right) {
                        LeftRotate(zpp, std::move(zpp->left));
                        zp = zpp->left.get();
                    }
                    zp->color = Color::Black;
                    zpp->color = Color::Red;
                    s.pop();
                    auto zppp = s.top();
                    s.push(zp);
                    if (!zppp) {
                        RightRotate(zppp, std::move(root));
                    } else if (zpp == zppp->left.get()) {
                        RightRotate(zppp, std::move(zppp->left));
                    } else {
                        RightRotate(zppp, std::move(zppp->right));
                    }
                }
            } else {
                auto y = zpp->left.get();
                if (y && y->color == Color::Red) {
                    zp->color = Color::Black;
                    y->color = Color::Black;
                    zpp->color = Color::Red;
                    s.pop();
                } else {
                    if (z == zp->left) {
                        RightRotate(zpp, std::move(zpp->right));
                        zp = zpp->right.get();
                    }
                    zp->color = Color::Black;
                    zpp->color = Color::Red;
                    s.pop();
                    auto zppp = s.top();
                    s.push(zp);
                    if (!zppp) {
                        LeftRotate(zppp, std::move(root));
                    } else if (zpp == zppp->left.get()) {
                        LeftRotate(zppp, std::move(zppp->left));
                    } else {
                        LeftRotate(zppp, std::move(zppp->right));
                    }
                }
            }
        }
        root->color = Color::Black;
    }

};

template <typename T>
std::ostream& operator<<(std::ostream& os, Node<T>* node) {
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

template <typename T>
std::ostream& operator<<(std::ostream& os, const RBTree<T>& tree) {
    os << tree.root.get();
    return os;
}

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

}