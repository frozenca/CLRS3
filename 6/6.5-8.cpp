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
void buildMaxHeap(std::pair<std::vector<T>&, size_t>& A) {
    A.second = A.first.size();
    for (size_t i = A.first.size() / 2; i < A.first.size(); i--) {
        maxHeapify(A, i);
    }
}

template <typename T>
void heapSort(std::vector<T>& v) {
    std::pair<std::vector<T>&, size_t> A = {v, 0};
    buildMaxHeap(A);
    for (size_t i = A.first.size() - 1; i >= 1; i--) {
        std::swap(A.first[i], A.first[0]);
        A.second--;
        maxHeapify(A, 0);
    }
}

template <typename T>
T heapMaximum(const std::pair<std::vector<T>&, size_t>& v) {
    return v[0];
}

template <typename T>
T heapExtractMax(std::pair<std::vector<T>&, size_t>& A) {
    if (A.second < 1) {
        throw std::underflow_error("heap underflow");
    }
    T maxElem = A.first[0];
    A.first[0] = A.first[A.second - 1];
    A.second--;
    maxHeapify(A, 0);
    return maxElem;
}

template <typename T>
void heapIncreaseKey(std::pair<std::vector<T>&, size_t>& A, size_t i, const T& key) {
    if (key < A.first[i]) {
        throw std::runtime_error("new key is smaller than current key");
    }
    while (i > 0 && A.first[parent(i)] < key) {
        A.first[i] = A.first[parent(i)];
        i = parent(i);
    }
}

template <typename T>
void maxHeapDelete(std::pair<std::vector<T>&, size_t>& A, size_t i) {
    if (A.first[i] > A.first[A.second - 1]) {
        A.first[i] = A.first[A.second - 1];
        maxHeapify(A, i);
    } else {
        heapIncreaseKey(A, i, A.first[A.second - 1]);
    }
    A.second--;
    A.first.pop_back();
}

int main() {
    std::vector<int> v {3, 1, 4, 1, 5, 9};
    sr::make_heap(v);
    std::pair<std::vector<int>&, size_t> A = {v, v.size()};
    maxHeapDelete(A, 2);
    assert(sr::is_heap(v));
}