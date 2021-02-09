#include <cassert>
#include <iostream>
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
public:
    std::shared_ptr<ListNode<T>> nil;
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

template <typename T>
List<T> Union(List<T>& s1, List<T>& s2) {
    List<T> s;
    auto s1_head = s1.nil->next;
    s.nil->next = s1_head;
    s1_head->prev = s.nil;
    s1.nil->next = nullptr;
    auto s1_tail = s1.nil->prev.lock();
    auto s2_head = s2.nil->next;
    s1_tail->next = s2_head;
    s2_head->prev = s1_tail;
    auto s2_tail = s2.nil->prev.lock();
    s2_tail->next = s.nil;
    s.nil->prev = s2_tail;
    return s;
}

int main() {
    auto p1 = std::make_shared<ListNode<int>>(1);
    auto p2 = std::make_shared<ListNode<int>>(2);
    auto p3 = std::make_shared<ListNode<int>>(3);
    auto p4 = std::make_shared<ListNode<int>>(4);
    auto p5 = std::make_shared<ListNode<int>>(5);
    List<int> l;
    l.ListInsert(p1);
    assert(l.ListSearch(1));
    l.ListInsert(p2);
    l.ListInsert(p3);
    l.ListDelete(p2);
    assert(!l.ListSearch(2));
    List<int> l2;
    l2.ListInsert(p4);
    l2.ListInsert(p5);
    auto l3 = Union(l, l2);
    assert(l3.ListSearch(3));
    assert(!l3.ListSearch(2));
    assert(l3.ListSearch(4));
    assert(l3.ListSearch(5));
    assert(l3.ListSearch(1));

}