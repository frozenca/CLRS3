#include <cassert>
#include <vector>
#include <iostream>
#include <limits>
#include <utility>

template <typename T>
void selectionSort(std::vector<T>& A) {
    for (size_t i = 0; i < A.size() - 1; i++) {
        T min_value = A[i];
        size_t argmax = i;
        for (size_t j = i + 1; j < A.size(); j++) {
            if (min_value > A[j]) {
                min_value = A[j];
                argmax = j;
            }
        }
        std::swap(A[i], A[argmax]);
    }
}

int main() {
    std::vector<int> v {5, 2, 3, 1, 4};
    selectionSort(v);
    for (auto n : v) {
        std::cout << n << ' ';
    }
}