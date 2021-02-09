#include <cassert>
#include <iostream>
#include <forward_list>

template <typename T>
void Insert(std::forward_list<T>& l, const T& key) {
    l.push_front(key);
}

template <typename T>
void Delete(std::forward_list<T>& l, typename std::forward_list<T>::iterator it) {
    auto it2 = it;
    ++it2;
    *it = *it2;
    l.erase_after(it);
}

int main() {

}