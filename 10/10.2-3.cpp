#include <cassert>
#include <iostream>
#include <forward_list>

template <typename T>
struct ForwardList {
    std::forward_list<T> data;
    std::forward_list<T>::iterator it;

    ForwardList() {it = data.begin();}

    void Enqueue(const T& x) {
        data.insert_after(it);
        auto it2 = it;
        ++it2;
        it = it2;
    }

    void Dequeue() {
        data.pop_front();
    }
};




int main() {

}