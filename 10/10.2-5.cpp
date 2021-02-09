#include <cassert>
#include <iostream>
#include <memory>

template <typename T>
struct ListNode {
    T key;
    std::shared_ptr<ListNode<T>> next;
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
    }

    void ListDelete(const T& k) {
        auto prev = nil;
        while (prev->next->key != k) {
            if (prev->next == nil) {
                throw std::runtime_error("Key does not exist");
            }
            prev = prev->next;
        }
        prev->next = prev->next->next;
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

    void ListInsert(const T& k) {
        auto x = std::make_shared<ListNode<T>>(k);
        x->next = nil->next;
        nil->next = x;
    }
};

int main() {
    List<int> l;
    l.ListInsert(1);
    assert(l.ListSearch(1));
    l.ListInsert(2);
    l.ListInsert(3);
    l.ListDelete(2);
    assert(!l.ListSearch(2));

}