#include <cassert>
#include <iostream>
#include <stdexcept>
#include <stack>

template <typename T>
struct Queue {
    std::stack<T> A;
    std::stack<T> B;

    void Enqueue(const T& x) {
        A.push(x);
    }

    T Dequeue() {
        if (B.empty()) {
            while (!A.empty()) {
                T x = A.top();
                A.pop();
                B.push(x);
            }
        }
        T x = B.top();
        B.pop();
        return x;
    }
};


int main() {

}