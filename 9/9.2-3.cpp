#include <cassert>
#include <random>
#include <iostream>
#include <vector>

std::mt19937 gen(std::random_device{}());

template <typename T>
size_t partition(std::vector<T>& A, size_t p, size_t r) {
    T x = A[r];
    size_t i = p;
    for (size_t j = p; j < r; j++) {
        if (A[j] <= x) {
            std::swap(A[i], A[j]);
            i++;
        }
    }
    std::swap(A[i], A[r]);
    return i;
}

template <typename T>
size_t randomizedPartition(std::vector<T>& A, size_t p, size_t r) {
    std::uniform_int_distribution<> dst (p, r);
    size_t i = dst(gen);
    std::swap(A[i], A[r]);
    return partition(A, p, r);
}

template <typename T>
T randomizedSelectIterative(std::vector<T>& A, size_t p, size_t r, size_t i) {
    while (true) {
        if (p == r) {
            return A[p];
        }
        size_t q = randomizedPartition(A, p, r);
        size_t k = q - p + 1;
        if (i == k) {
            return A[q];
        } else if (i < k) {
            r = q;
        } else {
            p = q;
            i -= k;
        }
    }
}

int main() {
    std::vector<int> v {4, 2, 1, 5, 3, 6};
    std::cout << randomizedSelectIterative(v, 0, v.size() - 1, v.size() / 2);

}