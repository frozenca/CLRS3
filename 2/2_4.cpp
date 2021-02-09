#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
size_t merge(std::vector<T>& A, size_t p, size_t q, size_t r) {
    size_t i = p;
    size_t j = q;
    size_t count = 0;
    std::vector<T> B (1 + r - p);
    size_t k = 0;
    while (i < q && j <= r) {
        if (A[i] < A[j]) {
            B[k++] = A[i++];
        } else {
            B[k++] = A[j++];
            count += q - i;
        }
    }
    while (i < q) {
        B[k++] = A[i++];
    }
    while (j <= r) {
        B[k++] = A[j++];
    }
    std::copy(B.begin(), B.end(), A.begin() + p);
    return count;
}

template <typename T>
size_t inversionsHelper(std::vector<T>& A, size_t p, size_t r) {
    if (p < r) {
        size_t q = (p + r) / 2;
        size_t count = 0;
        count += inversionsHelper(A, p, q);
        count += inversionsHelper(A, q + 1, r);
        count += merge(A, p, q + 1, r);
        return count;
    }
    return 0;
}

template <typename T>
size_t inversions(std::vector<T>& A) {
    return inversionsHelper(A, 0, A.size() - 1);
}


int main() {
    std::vector<int> v {5, 4, 3, 2, 1};
    std::cout << inversions(v);


}