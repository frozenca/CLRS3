#include <array>
#include <cstddef>
#include <memory>
#include <optional>

template <typename T>
class Node {
    bool value = false;
    std::optional<T> data;
    Node* parent = nullptr;
    std::size_t index = 0;
    std::size_t height = 0;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

public:
    Node(std::size_t height) : height{height},
                               left{height > 0 ? std::make_unique<Node>(this, height - 1, index) : nullptr},
                               right{height > 0 ? std::make_unique<Node>(this, height - 1, index + (1u << (height - 1))) : nullptr}
    {}

    Node(Node* parent, std::size_t height, std::size_t index) : parent{parent}, index{index}, height{height}
    {}

    void Insert(std::size_t ind, const T& dat) {
        value = true;
        if (height > 0 && ind >= right->index) {
            right->Insert(ind);
        } else if (height > 0) {
            left->Insert(ind);
        } else {
            data = dat;
        }
    }

    void Delete() {
        if (left || right) {
            return;
        }
        data = {};
        value = false;
        Node* curr = const_cast<Node*>(this);
        while (curr->parent && !curr->parent->left->value) {
            curr->parent->value = false;
            curr = curr->parent;
        }
    }

    [[nodiscard]] std::size_t Minimum() const {
        if (left && left->value) {
            return left->Minimum();
        } else if (right && right->value) {
            return right->Minimum();
        } else {
            return index;
        }
    }

    [[nodiscard]] std::size_t Maximum() const {
        if (right && right->value) {
            return right->Maximum();
        } else if (left && left->value) {
            return left->Maximum();
        } else {
            return index;
        }
    }

    [[nodiscard]] std::optional<std::size_t> Successor() const {
        Node* curr = const_cast<Node*>(this);
        while (curr->parent && curr == curr->parent->left.get()) {
            if (curr->parent->right->value) {
                return curr->parent->right->Minimum();
            }
            curr = curr->parent;
        }
        return {};
    }

    [[nodiscard]] std::optional<std::size_t> Predecessor() const {
        Node* curr = const_cast<Node*>(this);
        while (curr->parent && curr == curr->parent->right.get()) {
            if (curr->parent->left->value) {
                return curr->parent->left->Maximum();
            }
            curr = curr->parent;
        }
        return {};
    }

};


int main() {

}