#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

enum class Color {
    Black,
    White
};

template <typename T>
class Node {
    T element;
    Node* parent = this;
    std::size_t rank = 0;
    Node* ancestor = nullptr;
    Color color = Color::White;
    std::vector<std::unique_ptr<Node>> child;

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

template <typename T>
void LCA(Node<T>* u, const std::unordered_map<Node<T>*, std::vector<Node<T>*>>& P) {
    FindSet(u)->ancestor = u;
    for (auto&& v : u->child) {
        LCA(v.get());
        Union(u, v.get());
        FindSet(u)->ancestor = u;
    }
    u->color = Color::Black;
    for (auto v : P[u]) {
        if (v->color == Color::Black) {
            std::cout << "The LCA of " << u->element  << " and "
            << v->element << " is " << FindSet(v)->ancestor->element << '\n';
        }
    }
}

int main() {

}