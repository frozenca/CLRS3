#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <stdexcept>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

size_t parent(size_t i) {
    return i / 2;
}

size_t left(size_t i) {
    return 2 * i + 1;
}

size_t right(size_t i) {
    return 2 * i + 2;
}

template <typename T>
void maxHeapify(std::pair<std::vector<T>&, size_t>& A, size_t i) {
    auto l = left(i);
    auto r = right(i);
    size_t largest = i;
    if (l < A.second && A.first[l] > A.first[i]) {
        largest = l;
    }
    if (r < A.second && A.first[r] > A.first[largest]) {
        largest = r;
    }
    if (largest != i) {
        std::swap(A.first[i], A.first[largest]);
        maxHeapify(A, largest);
    }
}

template <typename T>
void heapIncreaseKey(std::pair<std::vector<T>&, size_t>& A, size_t i, const T& key) {
    if (key < A.first[i]) {
        throw std::runtime_error("new key is smaller than current key");
    }
    A.first[i] = key;
    while (i > 0 && A.first[parent(i)] < A.first[i]) {
        std::swap(A.first[i], A.first[parent(i)]);
        i = parent(i);
    }
}

template <typename T>
void maxHeapInsert(std::pair<std::vector<T>&, size_t>& A, const T& key) {
    A.second++;
    if (A.first.size() < A.second) {
        A.first.push_back(std::numeric_limits<T>::min());
    } else {
        A.first[A.second - 1] = std::numeric_limits<T>::min();
    }
    heapIncreaseKey(A, A.second - 1, key);
}

template <typename T>
void buildMaxHeap2(std::pair<std::vector<T>&, size_t>& A) {
    A.second = 1;
    for (size_t i = 1; i < A.first.size(); i++) {
        maxHeapInsert(A, A.first[i]);
    }
}

int main() {
    std::vector<int> v {3, 1, 4, 1, 5, 9};
    std::pair<std::vector<int>&, size_t> A = {v, v.size()};
    buildMaxHeap2(A);
    assert(sr::is_heap(v));
}