#include <cassert>
#include <iostream>
#include <forward_list>

template <typename T>
void Push(std::forward_list<T>& l, const T& key) {
    l.push_front(key);
}

template <typename T>
T Pop(std::forward_list<T>& l) {
    T x = l.front();
    l.pop_front();
}

int main() {

}