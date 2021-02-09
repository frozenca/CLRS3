#include <cassert>
#include <iostream>
#include <utility>
#include <stdexcept>
#include <vector>

template <typename T>
struct Queue {
    std::vector<T> data;
    size_t head = 0;
    size_t tail = 0;
};

template <typename T>
void Enqueue(Queue<T>& q, const T& x) {
    if (q.head == q.tail + 1 || (q.head == 0 && q.tail == q.data.size() - 1)) {
        throw std::overflow_error("Queue overflow");
    }
    q.data[q.tail] = x;
    if (q.tail == q.data.size() - 1) {
        q.tail = 0;
    } else {
        q.tail++;
    }
}

template <typename T>
void Dequeue(Queue<T>& q) {
    if (q.head == q.tail) {
        throw std::underflow_error("Queue underflow");
    }
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