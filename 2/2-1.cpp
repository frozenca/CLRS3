#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

template <typename T>
void insertionSort(std::vector<T>& A, size_t p, size_t q) {
    assert(p <= A.size() && q <= A.size());
    for (size_t j = p + 1; j < q; j++) {
        T key = A[j];
        size_t i = j - 1;
        while (i >= p && i < q && A[i] > key) {
            A[i + 1] = A[i];
            i--;
        }
        A[i + 1] = key;
    }
}

template <typename T>
void merge(std::vector<T>& A, size_t p, size_t q, size_t r) {
    assert(p <= A.size() && q <= A.size() && r <= A.size() && p <= q && q <= r);
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

constexpr size_t SORT_CRITERION = 20;

template <typename T>
void divideSort(std::vector<T>& A, size_t p, size_t r) {
    assert(r >= p);
    if (r - p < SORT_CRITERION) {
        insertionSort(A, p, r);
    } else {
        size_t q = p + (r - p) / 2;
        divideSort(A, p, q);
        divideSort(A, q, r);
        merge(A, p, q, r);
    }
}

std::mt19937 gen(std::random_device{}());

int main() {
    size_t length = 10;
    while (length <= 1'000'000) {
        std::vector<int> A (length);
        std::iota(A.begin(), A.end(), 0);
        std::shuffle(A.begin(), A.end(), gen);

        auto time1 = std::chrono::steady_clock::now();
        divideSort(A, 0, A.size());
        auto time2 = std::chrono::steady_clock::now();
        assert(is_sorted(A.begin(), A.end()));
        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1);
        cout << "Sorting " << length << " elems using merge sort with criterion " << SORT_CRITERION << " : " << diff.count() << "us\n";

        std::shuffle(A.begin(), A.end(), gen);
        time1 = std::chrono::steady_clock::now();
        std::sort(A.begin(), A.end());
        time2 = std::chrono::steady_clock::now();
        diff = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1);
        cout << "Sorting " << length << " elems using standard sort : " << diff.count() << "us\n";
        length *= 10;
    }

}