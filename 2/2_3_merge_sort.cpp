#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
void merge(std::vector<T>& A, size_t p, size_t q, size_t r) {
    size_t i = p;
    size_t j = q;
    std::vector<T> B (r - p);
    for (size_t k = 0; k < B.size(); k++) {
        if (A[i] < A[j] || j >= r) {
            B[k] = A[i];
            i++;
        } else {
            B[k] = A[j];
            j++;
        }
    }
    std::copy(A.begin() + p, A.begin() + r, B.begin());
}

template <typename T>
void mergeSortHelper(std::vector<T>& A, size_t p, size_t r) {
    if (p < r) {
        size_t q = p + (r - p) / 2;
        mergeSortHelper(A, p, q);
        mergeSortHelper(A, q + 1, r);
        merge(A, p, q, r);
    }
}

template <typename T>
void mergeSort(std::vector<T>& A) {
    mergeSortHelper(A, 0, A.size() - 1);
}

int main() {
    std::vector<int> v {5, 4, 3, 2, 1};
    mergeSort(v);
    for (auto n : v) {
        std::cout << n << ' ';
    }


}