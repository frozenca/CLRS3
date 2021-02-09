#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include <utility>

std::mt19937 gen(std::random_device{}());

template <typename T>
struct BSTNode {
    T key;
    std::unique_ptr<BSTNode<T>> left;
    std::unique_ptr<BSTNode<T>> right;
    BSTNode<T>* parent;

    BSTNode(const T& key) : key {key}, parent {nullptr} {}
};

std::bernoulli_distribution dist (0.5);

template <typename T>
struct BST {
    std::unique_ptr<BSTNode<T>> root;

    BSTNode<T>* Search(BSTNode<T>* x, const T& key) {
        if (!x || x->key == key) {
            return x;
        }
        if (key < x->key) {
            return Search(x->left.get(), key);
        } else {
            return Search(x->right.get(), key);
        }
    }

    BSTNode<T>* Search(const T& key) {
        return Search(root.get(), key);
    }

    BSTNode<T>* Minimum(BSTNode<T>* x) {
        while (x->left) {
            x = x->left.get();
        }
        return x;
    }

    BSTNode<T>* Maximum(BSTNode<T>* x) {
        while (x->right) {
            x = x->right.get();
        }
        return x;
    }

    BSTNode<T>* Successor(BSTNode<T>* x) {
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

    BSTNode<T>* Predecessor(BSTNode<T>* x) {
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

    void Insert(const T& key) {
        auto z = std::make_unique<BSTNode<T>>(key);
        BSTNode<T>* y = nullptr;
        auto x = root.get();
        while (x) {
            y = x;
            if (key < x->key) {
                x = x->left.get();
            } else {
                x = x->right.get();
            }
        }
        z->parent = y;
        if (!y) {
            root = std::move(z);
        } else if (key < y->key) {
            y->left = std::move(z);
        } else {
            y->right = std::move(z);
        }
    }

    void InsertRecursive(BSTNode<T>* parent, BSTNode<T>* curr, std::unique_ptr<BSTNode<T>> z) {
        if (!curr) {
            z->parent = parent;
            if (z->key < parent->key) {
                parent->left = std::move(z);
            } else {
                parent->right = std::move(z);
            }
        } else if (z->key < curr->key) {
            InsertRecursive(curr, curr->left.get(), std::move(z));
        } else {
            InsertRecursive(curr, curr->right.get(), std::move(z));
        }
    }

    void Transplant(BSTNode<T>* u, std::unique_ptr<BSTNode<T>>&& v) {
        if (v) {
            v->parent = u->parent;
        }
        if (!u->parent) {
            root = std::move(v);
        } else if (u == u->parent->left.get()) {
            u->parent->left = std::move(v);
        } else {
            u->parent->right = std::move(v);
        }
    }

    void Delete(BSTNode<T>* z) {
        if (!z->left) {
            Transplant(z, std::move(z->right));
        } else if (!z->right) {
            Transplant(z, std::move(z->left));
        } else {
            BSTNode<T>* y1 = nullptr;
            std::unique_ptr<BSTNode<T>> y;
            if (dist(gen)) {
                y1 = z->right.get();
                if (!y1->left) {
                    y = std::move(z->right);
                } else {
                    while (y1->left) {
                        y1 = y1->left.get();
                    }
                    y = std::move(y1->parent->left);
                }
            } else {
                y1 = z->left.get();
                if (!y1->right) {
                    y = std::move(z->left);
                } else {
                    while (y1->right) {
                        y1 = y1->right.get();
                    }
                    y = std::move(y1->parent->right);
                }
            }
            if (y->parent != z) {
                Transplant(y.get(), std::move(y->right));
                y->right = std::move(z->right);
                y->right->parent = y.get();
            }
            auto yp = y.get();
            Transplant(z, std::move(y));
            yp->left = std::move(z->left);
            yp->left->parent = yp;
        }
    }

    void Delete(const T& key) {
        auto z = Search(key);
        Delete(z);
    }

};

template <typename T>
std::ostream& operator<<(std::ostream& os, BSTNode<T>* node) {
    if (node) {
        os << node->left.get();
        os << node->key << ' ';
        os << node->right.get();
    }
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const BST<T>& tree) {
    os << tree.root.get();
    return os;
}

int main() {
    BST<int> bst;
    bst.Insert(1);
    bst.Insert(2);
    bst.Insert(3);
    bst.Insert(4);
    bst.Insert(5);
    bst.Insert(6);
    bst.Delete(3);
    std::cout << bst << '\n';
    bst.Insert(3);
    std::cout << bst << '\n';


}