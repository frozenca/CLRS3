#include <cassert>
#include <iostream>
#include <utility>
#include <memory>

template <typename T>
struct NaryTreeNode {
    T key;
    std::unique_ptr<NaryTreeNode<T>> left_child;
    std::unique_ptr<NaryTreeNode<T>> right_sibling;

    NaryTreeNode(const T& key) : key {key} {}
};

template <typename T>
struct NaryTree {
    std::unique_ptr<NaryTreeNode<T>> root;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, NaryTreeNode<T>* node) {
    if (node) {
        os << node->key << ' ';
        auto curr = node->left_child.get();
        while (curr) {
            os << curr;
            curr = curr->right_sibling.get();
        }
    }
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const NaryTree<T>& tree) {
    os << tree.root.get();
    return os;
}

int main() {
    NaryTree<int> t;
    auto n1 = std::make_unique<NaryTreeNode<int>>(1);
    auto n2 = std::make_unique<NaryTreeNode<int>>(2);
    auto n3 = std::make_unique<NaryTreeNode<int>>(3);
    auto n4 = std::make_unique<NaryTreeNode<int>>(4);
    auto n5 = std::make_unique<NaryTreeNode<int>>(5);

    t.root = std::move(n1);
    t.root->left_child = std::move(n2);
    t.root->left_child->right_sibling = std::move(n3);
    t.root->left_child->right_sibling->right_sibling = std::move(n4);
    t.root->left_child->left_child = std::move(n5);

    std::cout << t;


}