#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
bool binarySearchRecursive(const std::vector<T>& A, size_t p, size_t q, const T& key) {
    if (p > q || q > A.size()) {
        return false;
    }
    size_t m = p + (q - p) / 2;
    if (A[m] > key) {
        return binarySearchRecursive(A, p, m - 1, key);
    } else if (A[m] < key) {
        return binarySearchRecursive(A, m + 1, q, key);
    } else {
        return true;
    }
}

template <typename T>
bool binarySearch(const std::vector<T>& A, const T& key) {
    if (A.empty()) return false;
    return binarySearchRecursive(A, 0, A.size() - 1, key);
}

int main() {
    std::vector<int> v {1, 2, 4, 6, 7};
    assert(!binarySearch(v, 0));
    assert(binarySearch(v, 1));
    assert(binarySearch(v, 2));
    assert(!binarySearch(v, 3));
    assert(binarySearch(v, 4));
    assert(!binarySearch(v, 5));
    assert(binarySearch(v, 6));
    assert(binarySearch(v, 7));
    assert(!binarySearch(v, 8));
}