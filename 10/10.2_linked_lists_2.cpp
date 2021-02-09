#include <cassert>
#include <iostream>
#include <utility>
#include <memory>

template <typename T>
struct ListNode {
    T key;
    std::shared_ptr<ListNode<T>> next;
    std::weak_ptr<ListNode<T>> prev;
    ListNode(const T& key) : key {key} {}
};

template <typename T>
struct List {
private:
    std::shared_ptr<ListNode<T>> nil;
public:
    List() {
        nil = std::make_shared<ListNode<T>>(T());
        nil->next = nil;
        nil->prev = nil;
    }

    void ListDelete(std::shared_ptr<ListNode<T>>& x) {
        x->prev.lock()->next = x->next;
        x->next->prev = x->prev;
        x.reset();
    }

    std::shared_ptr<ListNode<T>> ListSearch(const T& k) {
        auto x = nil->next;
        while (x != nil && x->key != k) {
            x = x->next;
        }
        if (x == nil) {
            return nullptr;
        }
        return x;
    }

    void ListInsert(std::shared_ptr<ListNode<T>>& x) {
        x->next = nil->next;
        nil->next->prev = x;
        nil->next = x;
        x->prev = nil;
    }
};

int main() {
    auto p1 = std::make_shared<ListNode<int>>(1);
    auto p2 = std::make_shared<ListNode<int>>(2);
    auto p3 = std::make_shared<ListNode<int>>(3);
    List<int> l;
    l.ListInsert(p1);
    assert(l.ListSearch(1));
    l.ListInsert(p2);
    l.ListInsert(p3);
    l.ListDelete(p2);
    assert(!l.ListSearch(2));

}