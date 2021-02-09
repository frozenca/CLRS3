#include <algorithm>
#include <cassert>
#include <iostream>
#include <array>
#include <stdexcept>

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
struct GC {
    std::array<size_t, N> prev;
    std::array<size_t, N> next;
    std::array<T, N> key = {0};

    Stack<size_t, N> free = 0;
    size_t head = 0;

    GC() {
        for (size_t i = 0; i < N; i++) {
            prev[i] = i - 1;
            next[i] = i + 1;
        }
        next[N - 1] = -1;
    }

    size_t AllocateObject() {
        if (StackEmpty(free)) {
            throw std::runtime_error("out of space");
        } else {
            return Pop(free);
        }
    }

    void FreeObject(size_t x) {
        auto p = free.top + 1;
        next[prev[p]] = x;
        prev[next[p]] = x;
        key[x] = key[p];
        prev[x] = prev[p];
        next[x] = next[p];
        Push(free, p);
    }
};


int main() {

}