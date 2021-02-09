#include <cassert>
#include <iostream>
#include <vector>
#include <utility>

template <typename T, typename Comp = std::less<T>>
void bubbleSort(std::vector<T>& A, Comp comp = Comp()) {
    for (size_t i = 0; i < A.size() - 1; i++) {
        for (size_t j = A.size() - 1; j > i; j--) {
            if (comp(A[j], A[j - 1])) {
                std::swap(A[j], A[j - 1]);
            }
        }
    }
}

int main() {
    std::vector<int> v {2, 1, 7, 6, 3, 5};
    bubbleSort(v);
    for (auto n : v) {
        std::cout << n << ' ';
    }
}