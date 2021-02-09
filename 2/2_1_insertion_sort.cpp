#include <cassert>
#include <vector>
#include <iostream>

template <typename T, typename Comp = std::greater<T>>
void insertionSort(std::vector<T>& A, Comp comp = Comp()) {
    for (size_t j = 1; j < A.size(); j++) {
        T key = A[j];
        size_t i = j - 1;
        while (i < A.size() && comp(A[i], key)) {
            A[i + 1] = A[i];
            i--;
        }
        A[i + 1] = key;
    }
}

int main() {
    std::vector<int> v {5, 2, 3, 1, 4};
    insertionSort(v);
    for (auto n : v) {
        std::cout << n << ' ';
    }
}