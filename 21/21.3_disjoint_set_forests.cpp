#include <cstddef>

template <typename T>
class Node {
    T element;
    Node* parent = this;
    std::size_t rank = 0;

public:
    Node(const T& element) : element(element) {}
};

template <typename T>
Node<T> MakeSet(const T& element) {
    Node<T> node(element);
    return node;
}

template <typename T>
void Link(Node<T>* x, Node<T>* y) {
    if (!x || !y) {
        return;
    }
    if (x->rank > y->rank) {
        y->parent = x;
    } else {
        x->parent = y;
        if (x->rank == y->rank) {
            y->rank++;
        }
    }
}

template <typename T>
void Union(Node<T>* x, Node<T>* y) {
    Link(FindSet(x), FindSet(y));
}

template <typename T>
Node<T>* FindSet(Node<T>* x) {
    if (!x) {
        return nullptr;
    }
    if (x != x->parent) {
        x->parent = FindSet(x->parent);
    }
    return x->parent;
}

int main() {

}