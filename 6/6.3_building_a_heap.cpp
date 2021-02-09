#include <algorithm>
#include <cassert>
#include <cmath>
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
void maxHeapify(std::vector<T>& A, size_t i) {
    auto l = left(i);
    auto r = right(i);
    size_t largest = i;
    if (l < A.size() && A[l] > A[i]) {
        largest = l;
    }
    if (r < A.size() && A[r] > A[largest]) {
        largest = r;
    }
    if (largest != i) {
        std::swap(A[i], A[largest]);
        maxHeapify(A, largest);
    }
}

template <typename T>
void buildMaxHeap(std::vector<T>& A) {
    for (size_t i = A.size() / 2; i < A.size(); i--) {
        maxHeapify(A, i);
    }
}

int main() {
    std::vector<int> v {3, 1, 4, 1, 5, 9};
    buildMaxHeap(v);
    assert(sr::is_heap(v));
}