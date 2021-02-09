#include <cassert>
#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>

struct Node {
    bool hasValue = false;
    std::string value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    Node() = default;
    Node(std::string value) : value {std::move(value)} {};
};

struct RadixTree {
    std::unique_ptr<Node> root;

    RadixTree () {
        root = std::make_unique<Node>();
    }

    void Insert(const std::string& value) {
        auto node = root.get();
        for (char c : value) {
            if (c == '0') {
                if (!node->left) {
                    node->left = std::make_unique<Node>();
                }
                node = node->left.get();
            } else if (c == '1') {
                if (!node->right) {
                    node->right = std::make_unique<Node>();
                }
                node = node->right.get();
            } else {
                throw std::invalid_argument("Not a bit string");
            }
        }
        node->hasValue = true;
        node->value = value;
    }
};

std::ostream& operator<<(std::ostream& os, Node* node) {
    if (node) {
        if (node->hasValue) {
            os << node->value << ' ';
        }
        os << node->left.get();
        os << node->right.get();
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const RadixTree& tree) {
    os << tree.root.get();
    return os;
}

int main() {
    RadixTree tree;
    tree.Insert("0");
    tree.Insert("10");
    tree.Insert("011");
    tree.Insert("100");
    tree.Insert("1011");
    std::cout << tree;
}