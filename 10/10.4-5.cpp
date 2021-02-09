#include <cassert>
#include <iostream>
#include <memory>

template <typename T>
struct BinaryTreeNode {
    T key;
    std::unique_ptr<BinaryTreeNode<T>> left;
    std::unique_ptr<BinaryTreeNode<T>> right;
    BinaryTreeNode<T>* parent;

    BinaryTreeNode(const T& key) : key {key} {}
};

template <typename T>
struct BinaryTree {
    std::unique_ptr<BinaryTreeNode<T>> root;

};

template <typename T>
std::ostream& operator<<(std::ostream& os, const BinaryTree<T>& tree) {

    enum class From {
        Parent,
        LeftChild,
        RightChild
    };

    From pred = From::Parent;
    auto curr = tree.root.get();
    while (curr) {
        if (pred == From::Parent) {
            os << curr->key << ' ';
            if (curr->left) {
                curr = curr->left.get();
                pred = From::Parent;
            } else {
                pred = From::LeftChild;
            }
        } else if (pred == From::LeftChild) {
            if (curr->right) {
                curr = curr->right.get();
                pred = From::Parent;
            } else {
                pred = From::RightChild;
            }
        } else {
            if (!curr->parent) {
                curr = nullptr;
                break;
            }
            if (curr == curr->parent->left.get()) {
                pred = From::LeftChild;
            } else {
                pred = From::RightChild;
            }
            curr = curr->parent;
        }
    }

    return os;
}

int main() {
    BinaryTree<int> t;
    auto n1 = std::make_unique<BinaryTreeNode<int>>(1);
    auto n2 = std::make_unique<BinaryTreeNode<int>>(2);
    auto n3 = std::make_unique<BinaryTreeNode<int>>(3);

    t.root = std::move(n1);
    t.root->left = std::move(n2);
    t.root->left->parent = t.root.get();
    t.root->right = std::move(n3);
    t.root->right->parent = t.root.get();

    std::cout << t;

}