#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <stdexcept>
#include <vector>

constexpr size_t D = 4;

size_t parent(size_t i) {
    if (i == 0) return 0;
    return (i - 1) / D;
}

size_t dAryChild(size_t i, size_t j) {
    return D * i + j + 1;
}

template <typename T>
void maxHeapify(std::pair<std::vector<T>&, size_t>& A, size_t i) {
    size_t largest = i;
    for (size_t j = 0; j < D; j++) {
        size_t child = dAryChild(i, j);
        if (child < A.second && A.first[child] > A.first[largest]) {
            largest = child;
        }
    }
    if (largest != i) {
        std::swap(A.first[i], A.first[largest]);
        maxHeapify(A, largest);
    }
}

template <typename T>
void buildMaxHeap(std::pair<std::vector<T>&, size_t>& A) {
    A.second = A.first.size();
    for (size_t i = A.first.size() / D; i < A.first.size(); i--) {
        maxHeapify(A, i);
    }
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

int main() {
    std::vector<int> v {1, 10, 2, 6, 9, 5, 3, 8, 7, 4};
    std::pair<std::vector<int>&, size_t> A = {v, v.size()};
    buildMaxHeap(A);
    for (auto n : v) {
        std::cout << n << ' ';
    }
}