#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

template <typename T = int>
struct Node {
    Node* parent = this;
    std::list<Node*>& lref;
    std::list<Node*>::iterator it;
    T value = T{};
public:
    Node(const T& value, std::list<Node*>& l) : value {value}, lref {l} {}
};

template <typename T = int>
std::list<Node<T>*> MakeSet(const T& value) {
    std::list<Node<T>*> l;
    l.push_front(new Node(value, l));
    l.front()->it = l.begin();
    return l;
}

template <typename T = int>
Node<T>* FindSet(Node<T>* x) {
    if (x != x->parent) {
        x->parent = FindSet(x->parent);
    }
    return x->parent;
}

template <typename T = int>
Node<T>* Link(Node<T>* x, Node<T>* y) {
    y->parent = x;
    y->lref.erase(y->it);
    y->lref = x->lref;
    y->it = x->it;
    y->value = x->value;
    return x;
}

template <typename T = int>
Node<T>* Union(Node<T>* x, Node<T>* y) {
    return Link(FindSet(x), FindSet(y));
}

std::vector<int> OfflineMinimum (const std::vector<int>& v, int m, int n) {
    std::list<Node<int>*> l;
    std::vector<Node<int>*> vp (n + 1);
    int prev = -1;
    int call_index = 1;
    for (auto num : v) {
        if (prev == -1) {
            if (num != -1) {
                auto s = MakeSet(call_index);
                vp[num] = *s.begin();
                l.splice(l.end(), s);
            } else {
                auto s = MakeSet(call_index);
                l.splice(l.end(), s);
            }
        } else if (num != -1) {
            auto l2 = MakeSet(call_index);
            vp[num] = *l2.begin();
            auto last = *l.rbegin();
            Union(last, *l2.begin());
        }
        if (num == -1) {
            call_index++;
        }
        prev = num;
    }

    std::vector<int> extracted (m);
    for (int i = 1; i <= n; i++) {
        auto jp = FindSet(vp[i]);
        auto j = jp->value;
        if (j != m + 1) {
            extracted[j - 1] = i;
            auto curr = jp->it;
            ++curr;
            auto lp = *curr;
            lp = Link(lp, jp);
        }
    }
    return extracted;
}

int main() {
    std::vector<int> v {4, 8, -1, 3, -1, 9, 2, 6, -1, -1, -1, 1, 7, -1, 5};
    auto v2 = OfflineMinimum(v, 6, 9);

    for (auto n : v2) {
        std::cout << n << ' ';
    }


}