#include <cassert>
#include <iostream>
#include <utility>

template <typename T>
struct ListNode {
    T key;
    long np;
    ListNode(const T& key) : key {key}, np {0} {}
};

template <typename T>
struct List {
    ListNode<T>* head;
    ListNode<T>* tail;

    List() : head {nullptr}, tail {nullptr} {}

    ~List() {
        long prev = 0;
        auto y = head;
        while (reinterpret_cast<long>(y)) {
            long next = y->np;
            head = reinterpret_cast<ListNode<T> *>(next);
            if (next) {
                auto next_ptr = reinterpret_cast<ListNode<T> *>(next);
                next_ptr->np = next_ptr->np ^ reinterpret_cast<long>(y) ^ prev;
            } else {
                tail = reinterpret_cast<ListNode<T> *>(prev);
            }
            auto temp = y;
            delete temp;
            y = reinterpret_cast<ListNode<T> *>(next);
        }
    }

    ListNode<T>* ListSearch(const T& k) {
        long prev = 0;
        auto x = head;
        while (reinterpret_cast<long>(x) && x->key != k) {
            long next = prev ^ x->np;
            prev = reinterpret_cast<long>(x);
            x = reinterpret_cast<ListNode<T>*>(next);
        }
        return x;
    }

    void ListInsert(const T& k) {
        auto x = new ListNode<T>(k);
        if (!head) {
            head = x;
            tail = x;
        } else if (head == tail) {
            tail = x;
            head->np = reinterpret_cast<long>(x);
            tail->np = reinterpret_cast<long>(head);
        } else {
            tail->np = tail->np ^ reinterpret_cast<long>(x);
            x->np = reinterpret_cast<long>(tail);
            tail = x;
        }
    }

    void ListDelete(const T& k) {
        auto y = head;
        long prev = 0;
        while (reinterpret_cast<long>(y)) {
            long next = prev ^ y->np;
            if (y->key != k) {
                prev = reinterpret_cast<long>(y);
                y = reinterpret_cast<ListNode<T>*>(next);
            } else {
                if (prev) {
                    auto prev_ptr = reinterpret_cast<ListNode<T>*>(prev);
                    prev_ptr->np = prev_ptr->np ^ reinterpret_cast<long>(y) ^ next;
                } else {
                    head = reinterpret_cast<ListNode<T>*>(next);
                }
                if (next) {
                    auto next_ptr = reinterpret_cast<ListNode<T>*>(next);
                    next_ptr->np = next_ptr->np ^ reinterpret_cast<long>(y) ^ prev;
                } else {
                    tail = reinterpret_cast<ListNode<T>*>(prev);
                }
                auto temp = y;
                delete temp;
                y = reinterpret_cast<ListNode<T>*>(next);
            }
        }
    }

    void ListReverse() {
        std::swap(head, tail);
    }
};

int main() {
    List<int> l;
    l.ListInsert(1);
    assert(l.ListSearch(1));
    l.ListInsert(2);
    l.ListInsert(3);
    assert(l.ListSearch(2));
    l.ListDelete(2);
    assert(!l.ListSearch(2));
    assert(l.ListSearch(3));
    assert(l.ListSearch(1));
    l.ListDelete(3);
    assert(!l.ListSearch(3));
    assert(l.ListSearch(1));
    l.ListDelete(1);
    assert(!l.ListSearch(1));

}