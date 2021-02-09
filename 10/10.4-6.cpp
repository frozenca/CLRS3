#include <cassert>
#include <iostream>
#include <utility>
#include <memory>

template <typename T>
struct NaryTreeNode {
    T key;
    NaryTreeNode<T>* left_child;
    NaryTreeNode<T>* right_sibling;
    bool is_rightmost = false;

    NaryTreeNode(const T& key) : key {key} {}

    NaryTreeNode<T>* parent() {
        auto curr = this;
        while (!curr->is_rightmost) {
            curr = curr->right_sibling;
        }
        return curr->right_sibling;
    }
};

template <typename T>
struct NaryTree {
    std::unique_ptr<NaryTreeNode<T>> root;
};

int main() {

}