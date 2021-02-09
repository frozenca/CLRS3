#include <cassert>
#include <iostream>
#include <utility>
#include <stdexcept>
#include <array>

template <typename T, size_t N>
struct Stack {
    std::array<T, N> data;
    size_t top = 0;
};

template <typename T, size_t N>
bool StackEmpty(const Stack<T, N>& s) {
    return s.top == 0;
}

template <typename T, size_t N>
void Push(Stack<T, N>& s, const T& x) {
    s.data[s.top++] = x;
}

template <typename T, size_t N>
T Pop(Stack<T, N>& s) {
    if (StackEmpty(s)) {
        throw std::underflow_error("Stack underflow");
    } else {
        return s.data[--s.second];
    }
}

template <typename T, size_t N>
struct Queue {
    std::array<T, N> data;
    size_t head = 0;
    size_t tail = 0;
};

template <typename T, size_t N>
void Enqueue(Queue<T, N>& q, const T& x) {
    q.data[q.tail] = x;
    if (q.tail == q.data.size() - 1) {
        q.tail = 0;
    } else {
        q.tail++;
    }
}

template <typename T, size_t N>
void Dequeue(Queue<T, N>& q) {
    T x = q.data[q.head];
    if (q.head == q.data.size() - 1) {
        q.head = 0;
    } else {
        q.head++;
    }
    return x;
}

int main() {

}