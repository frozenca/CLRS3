#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

template <typename T>
struct BSTNode {
    T key;
    std::unique_ptr<BSTNode<T>> left;
    std::unique_ptr<BSTNode<T>> right;
    BSTNode<T>* parent;

    BSTNode(const T& key) : key {key}, parent {nullptr} {}
};

template <typename T>
struct BST {
    std::unique_ptr<BSTNode<T>> root;

    BSTNode<T>* Search(const BSTNode<T>* x, const T& key) {
        if (!x || x->key == key) {
            return x;
        }
        if (key < x->key) {
            return Search(x->left.get(), key);
        } else {
            return Search(x->right.get(), key);
        }
    }

    BSTNode<T>* SearchIterative(const BSTNode<T>* x, const T& key) {
        while (x && x->key != key) {
            if (key < x->key) {
                x = x->left.get();
            } else {
                x = x->right.get();
            }
        }
        return x;
    }

    BSTNode<T>* Search(const T& key) {
        return Search(root, key);
    }

    BSTNode<T>* Minimum(const BSTNode<T>* x) {
        while (x->left) {
            x = x->left.get();
        }
        return x;
    }

    BSTNode<T>* MinimumRecursive(const BSTNode<T>* x) {
        if (x->left) {
            return MinimumRecursive(x->left.get());
        }
        return x;
    }

    BSTNode<T>* Maximum(const BSTNode<T>* x) {
        while (x->right) {
            x = x->right.get();
        }
        return x;
    }

    BSTNode<T>* MaximumRecursive(const BSTNode<T>* x) {
        if (x->right) {
            return MaximumRecursive(x->right.get());
        }
        return x;
    }

    BSTNode<T>* Successor(const BSTNode<T>* x) {
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

    BSTNode<T>* Predecessor(const BSTNode<T>* x) {
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
};


int main() {

}e