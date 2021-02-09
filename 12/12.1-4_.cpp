#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

template <typename T>
struct BinaryTreeNode {
    T key;
    std::unique_ptr<BinaryTreeNode<T>> left;
    std::unique_ptr<BinaryTreeNode<T>> right;

    BinaryTreeNode(const T& key) : key {key} {}
};

template <typename T>
struct BinaryTree {
    std::unique_ptr<BinaryTreeNode<T>> root;

};

template <typename T>
std::ostream& operator<<(std::ostream& os, BinaryTreeNode<T>* node) {
    if (node) {
        // postorder
        os << node->left.get();
        os << node->right.get();
		os << node->key << ' ';
    }
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const BinaryTree<T>& tree) {
    os << tree.root.get();
    return os;
}

int main() {
    BinaryTree<int> t;
    auto n1 = std::make_unique<BinaryTreeNode<int>>(1);
    auto n2 = std::make_unique<BinaryTreeNode<int>>(2);
    auto n3 = std::make_unique<BinaryTreeNode<int>>(3);

    t.root = std::move(n1);
    t.root->left = std::move(n2);
    t.root->right = std::move(n3);

    std::cout << t;

}