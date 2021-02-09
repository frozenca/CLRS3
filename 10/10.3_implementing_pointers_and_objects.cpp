#include <algorithm>
#include <cassert>
#include <iostream>
#include <array>
#include <stdexcept>

template <typename T, size_t N>
struct GC {
    std::array<size_t, N> prev;
    std::array<size_t, N> next;
    std::array<T, N> key = {0};

    size_t free = 0;
    size_t head = 0;

    GC() {
        for (size_t i = 0; i < N; i++) {
            prev[i] = i - 1;
            next[i] = i + 1;
        }
        next[N - 1] = -1;
    }

    size_t AllocateObject() {
        if (free == -1) {
            throw std::runtime_error("out of space");
        } else {
            size_t x = free;
            free = next[x];
            return x;
        }
    }

    void FreeObject(size_t x) {
        next[x] = free;
        free = x;
    }
};


int main() {

}