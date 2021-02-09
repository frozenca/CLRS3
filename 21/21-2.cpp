#include <cstddef>

template <typename T>
class Node {
    T element;
    Node* parent = this;
    std::size_t rank = 0;
    std::size_t d = 0;

public:
    Node(const T& element) : element(element) {}
};

template <typename T>
Node<T> MakeTree(const T& element) {
    Node<T> node(element);
    return node;
}

template <typename T>
Node<T>* FindRoot(Node<T>* v) {
    if (v->parent != v->parent->parent) {
        auto y = v->parent;
        v->parent = FindRoot(y);
        v->d += y->d;
    }
    return v->parent;
}

template <typename T>
std::size_t FindDepth(Node<T>* v) {
    FindRoot(v);
    if (v == v->parent) {
        return v->d;
    } else {
        return v->d + v->parent->d;
    }
}

template <typename T>
void Graft(Node<T>* r, Node<T>* v) {
    auto rr = FindRoot(r);
    auto vr = FindRoot(v);
    auto z = FindDepth(v);
    if (rr->rank > vr->rank) {
        vr->parent = rr;
        rr->d += z + 1;
        vr->d -= rr->d;
    } else {
        rr->parent = vr;
        rr->d += z + 1 - vr->d;
        if (rr->rank == vr->rank) {
            vr->rank++;
        }
    }
}

int main() {

}