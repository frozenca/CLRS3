#include <cassert>
#include <iostream>
#include <utility>
#include <stdexcept>
#include <vector>

template <typename T>
struct TwoStack {
    std::vector<T> data;
    size_t top1;
    size_t top2;
    TwoStack(size_t n) : data(n), top1 {0}, top2 {n - 1} {assert(n);}

    void Push1(const T& x) {
        data[top1++] = x;
    }

    void Push2(const T& x) {
        data[top2--] = x;
    }

    T Pop1(const T& x) {
        if (top1 == 0) {
            throw std::underflow_error("Stack underflow");
        } else {
            return data[--top1];
        }
    }

    T Pop2(const T& x) {
        if (top2 == data.size() - 1) {
            throw std::underflow_error("Stack underflow");
        } else {
            return data[++top2];
        }
    }
};


int main() {

}