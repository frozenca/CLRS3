#include <cassert>
#include <iostream>
#include <utility>
#include <memory>

template <typename T>
struct ListNode {
    T key;
    ListNode<T>* prev;
    std::unique_ptr<ListNode<T>> next;
    ListNode(const T& key) : key {key} {}
};

template <typename T>
struct List {
    std::unique_ptr<ListNode<T>> head;
};

template <typename T>
ListNode<T>* ListSearch(const List<T>& L, const T& k) {
    auto x = L.head.get();
    while (x && x->key != k) {
        x = x->next.get();
    }
    return x;
}

template <typename T>
void ListInsert(List<T>& L, std::unique_ptr<ListNode<T>> x) {
    if (L.head) {
        L.head->prev = x.get();
    }
    x->next = std::move(L.head);
    L.head = std::move(x);
}

template <typename T>
void ListDelete(List<T>& L, ListNode<T>* x) {
    if (x->prev) {
        x->prev->next = std::move(x->next);
    } else {
        L.head = std::move(x->next);
    }
    if (x->next) {
        x->next->prev = x->prev;
    }
}

int main() {
    auto x1 = std::make_unique<ListNode<int>>(1);
    auto x2 = std::make_unique<ListNode<int>>(2);
    auto x3 = std::make_unique<ListNode<int>>(3);
    List<int> l;
    ListInsert(l, std::move(x1));
    assert(ListSearch(l, 1));
    ListInsert(l, std::move(x2));
    ListInsert(l, std::move(x3));
    ListDelete(l, ListSearch(l, 2));
    assert(!ListSearch(l, 2));

}