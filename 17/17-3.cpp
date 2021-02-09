#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

constexpr double alpha = 0.7;

template <typename T>
struct Node {
    T key;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node* parent;
    std::size_t size;

    Node(const T& key) : key {key}, parent {nullptr}, size {1} {}
};

template <typename T>
void inorderTraversal(const Node<T>* root, std::vector<T>& inorderWalk) {
    if (!root) return;
    inorderTraversal(root->left.get(), inorderWalk);
    inorderWalk.push_back(root->key);
    inorderTraversal(root->right.get(), inorderWalk);
}

template <typename T>
std::unique_ptr<Node<T>> makeBalancedTree(const std::vector<T>& inorderWalk, std::size_t start, std::size_t last) {
    if (start > last || last >= inorderWalk.size()) {
        return nullptr;
    }
    std::size_t mid = start + (last - start) / 2;
    T median = inorderWalk[mid];
    auto root = std::make_unique<Node<T>>(median);
    root->left = makeBalancedTree(inorderWalk, start, mid - 1);
    root->right = makeBalancedTree(inorderWalk, mid + 1, last);
    return root;
}

template <typename T>
std::unique_ptr<Node<T>> halfBalance(std::unique_ptr<Node<T>> root) {
    if (!root) {
        return nullptr;
    }
    std::vector<T> inorderWalk;
    inorderTraversal(root.get(), inorderWalk);
    return makeBalancedTree(inorderWalk, 0, inorderWalk.size() - 1);
}

template <typename T>
bool isBalanced(const Node<T>* node) {
    if (!node) return true;
    std::size_t lsize = node->left ? node->left->size : 0;
    std::size_t rsize = node->right ? node->right->size : 0;
    return lsize < node->size * alpha && rsize < node->size * alpha;
}

template <typename T>
struct BST {
    std::unique_ptr<Node<T>> root;

    Node<T>* Search(Node<T>* x, const T& key) {
        if (!x || x->key == key) {
            return x;
        }
        if (key < x->key) {
            return Search(x->left.get(), key);
        } else {
            return Search(x->right.get(), key);
        }
    }

    Node<T>* Search(const T& key) {
        return Search(root.get(), key);
    }

    Node<T>* Minimum(Node<T>* x) {
        while (x->left) {
            x = x->left.get();
        }
        return x;
    }

    Node<T>* Maximum(Node<T>* x) {
        while (x->right) {
            x = x->right.get();
        }
        return x;
    }

    Node<T>* Successor(Node<T>* x) {
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

    Node<T>* Predecessor(Node<T>* x) {
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
        auto z = std::make_unique<Node<T>>(key);
        Node<T>* y = nullptr;
        auto x = root.get();
        while (x) {
            y = x;
            y->size++;
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
        while (y && !isBalanced(y)) {
            y = y->parent;
        }
        if (!y) {
            root = halfBalance(std::move(root));
        } else if (!isBalanced(y->left.get())) {
            y->left = halfBalanced(std::move(y->left));
        } else {
            y->right = halfBalanced(std::move(y->right));
        }
    }

    void Transplant(Node<T>* u, std::unique_ptr<Node<T>>&& v) {
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

    void Delete(Node<T>* z) {
        Node<T>* y_ = nullptr;
        if (!z->left) {
            Transplant(z, std::move(z->right));
            y_ = z->parent;
        } else if (!z->right) {
            Transplant(z, std::move(z->left));
            y_ = z->parent;
        } else {
            auto y1 = z->right.get();
            std::unique_ptr<Node<T>> y;
            if (!y1->left) {
                y = std::move(z->right);
                y_ = z;
            } else {
                while (y1->left) {
                    y1 = y1->left.get();
                }
                y = std::move(y1->parent->left);
                y_ = y1->parent;
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
        auto curr = y_;
        while (curr) {
            curr->size--;
            curr = curr->parent;
        }
        while (y_ && !isBalanced(y_)) {
            y_ = y_->parent;
        }
        if (!y_) {
            root = halfBalance(std::move(root));
        } else if (!isBalanced(y_->left.get())) {
            y_->left = halfBalanced(std::move(y_->left));
        } else {
            y_->right = halfBalanced(std::move(y_->right));
        }
    }

    void Delete(const T& key) {
        auto z = Search(key);
        Delete(z);
    }

};

template <typename T>
std::ostream& operator<<(std::ostream& os, Node<T>* node) {
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

}