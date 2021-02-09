#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <stdexcept>
#include <vector>
#include <ranges>
#include <functional>

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
void minHeapify(std::pair<std::vector<T>&, size_t>& A, size_t i) {
    auto l = left(i);
    auto r = right(i);
    size_t smallest = i;
    if (l < A.second && A.first[l] < A.first[i]) {
        smallest = l;
    }
    if (r < A.second && A.first[r] < A.first[smallest]) {
        smallest = r;
    }
    if (smallest != i) {
        std::swap(A.first[i], A.first[smallest]);
        minHeapify(A, smallest);
    }
}

template <typename T>
void buildMinHeap(std::pair<std::vector<T>&, size_t>& A) {
    A.second = A.first.size();
    for (size_t i = A.first.size() / 2; i < A.first.size(); i--) {
        minHeapify(A, i);
    }
}

template <typename T>
T heapMinimum(const std::pair<std::vector<T>&, size_t>& v) {
    return v[0];
}

template <typename T>
T heapExtractMin(std::pair<std::vector<T>&, size_t>& A) {
    if (A.second < 1) {
        throw std::underflow_error("heap underflow");
    }
    T minElem = A.first[0];
    A.first[0] = A.first[A.second - 1];
    A.second--;
    minHeapify(A, 0);
    return minElem;
}

template <typename T>
void heapDecreaseKey(std::pair<std::vector<T>&, size_t>& A, size_t i, const T& key) {
    if (key > A.first[i]) {
        throw std::runtime_error("new key is larger than current key");
    }
    A.first[i] = key;
    while (i > 0 && A.first[parent(i)] > A.first[i]) {
        std::swap(A.first[i], A.first[parent(i)]);
        i = parent(i);
    }
}

template <typename T>
void minHeapInsert(std::pair<std::vector<T>&, size_t>& A, const T& key) {
    A.second++;
    A.first.push_back(std::numeric_limits<T>::max());
    heapDecreaseKey(A, A.second - 1, key);
}

int main() {
    std::vector<int> v {3, 1, 4, 1, 5, 9};
    sr::make_heap(v, std::greater<>());
    std::pair<std::vector<int>&, size_t> A = {v, v.size()};
    minHeapInsert(A, 6);
    assert(sr::is_heap(v, std::greater<>()));
}