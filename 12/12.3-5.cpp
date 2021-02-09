#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

template <typename T>
struct BSTNode {
    T key;
    std::unique_ptr<BSTNode<T>> left;
    std::unique_ptr<BSTNode<T>> right;
    BSTNode<T>* succ;

    BSTNode(const T& key) : key {key}, succ {nullptr} {}
};

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

    BSTNode<T>* Parent(BSTNode<T>* x) {
        if (x == root.get()) {
            return nullptr;
        }
        auto y = Maximum(x)->succ;
        if (!y) {
            y = root.get();
        } else {
            if (x == y->left.get()) {
                return y;
            }
            y = y->left.get();
        }
        while (x != y->right.get()) {
            y = y->right.get();
        }
        return y;
    }

    void Insert(const T& key) {
        auto z = std::make_unique<BSTNode<T>>(key);
        BSTNode<T>* y = nullptr;
        auto x = root.get();
        BSTNode<T>* pred = nullptr;
        while (x) {
            y = x;
            if (key < x->key) {
                x = x->left.get();
            } else {
                pred = x;
                x = x->right.get();
            }
        }
        if (!y) {
            root = std::move(z);
        } else if (key < y->key) {
            z->succ = y;
            if (pred) {
                pred->succ = z.get();
            }
            y->left = std::move(z);
        } else {
            z->succ = y->succ;
            y->succ = z.get();
            y->right = std::move(z);
        }
    }

    void Transplant(BSTNode<T>* u, std::unique_ptr<BSTNode<T>>&& v) {
        auto p = Parent(u);
        if (!p) {
            root = std::move(v);
        } else if (u == p->left.get()) {
            p->left = std::move(v);
        } else {
            p->right = std::move(v);
        }
    }

    void Delete(BSTNode<T>* z) {
        if (!z) {
            return;
        }
        BSTNode<T>* pred = nullptr;
        if (z->left) {
            pred = Maximum(z->left.get());
        } else {
            auto y = Parent(z);
            auto x = z;
            while (y && x == y->left.get()) {
                x = y;
                y = Parent(y);
            }
            pred = y;
        }
        pred->succ = z->succ;
        if (!z->left) {
            Transplant(z, std::move(z->right));
        } else if (!z->right) {
            Transplant(z, std::move(z->left));
        } else {
            auto y1 = z->right.get();
            std::unique_ptr<BSTNode<T>> y;
            if (!y1->left) {
                y = std::move(z->right);
            } else {
                while (y1->left) {
                    y1 = y1->left.get();
                }
                y = std::move(Parent(y1)->left);
            }
            if (Parent(y.get()) != z) {
                Transplant(y.get(), std::move(y->right));
                y->right = std::move(z->right);
            }
            y->left = std::move(z->left);
            Transplant(z, std::move(y));
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