#include <cassert>
#include <iostream>
#include <memory>
#include <stack>

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
std::ostream& operator<<(std::ostream& os, const BinaryTree<T>& tree) {
    std::stack<BinaryTreeNode<T>*> s;
    s.push(tree.root.get());
    while (!s.empty()) {
        auto x = s.top();
        while (x) {
            s.push(x->left.get());
            x = s.top();
        }
        s.pop();
        if (!s.empty()) {
            x = s.top();
            s.pop();
            os << x->key << ' ';
            s.push(x->right.get());
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
    t.root->right = std::move(n3);

    std::cout << t;

}