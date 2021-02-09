#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <queue>

template <typename T>
struct Stack {
    std::queue<T> A;
    std::queue<T> B;

    void Push(const T& x) {
        A.push(x);
    }

    T Pop() {
        while (A.size() > 1) {
            T x = A.front();
            A.pop();
            B.push(x);
        }
        T x = A.front();
        A.pop();
        std::swap(A, B);
        return x;
    }
};


int main() {

}