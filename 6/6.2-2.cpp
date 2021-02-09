#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

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
void minHeapify(std::vector<T>& A, size_t i) {
    auto l = left(i);
    auto r = right(i);
    size_t smallest = i;
    if (l < A.size() && A[l] < A[i]) {
        smallest = l;
    }
    if (r < A.size() && A[r] < A[smallest]) {
        smallest = r;
    }
    if (smallest != i) {
        std::swap(A[i], A[smallest]);
        minHeapify(A, smallest);
    }
}

int main() {

}	