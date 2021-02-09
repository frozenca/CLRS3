#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

void inplaceCountingSort(std::vector<size_t>& A, size_t k) {
    assert(*sr::max_element(A) <= k);
    std::vector<size_t> C (k + 1);
    for (auto n : A) {
        C[n]++;
    }
    for (size_t j = 1; j <= k; j++) {
        C[j] += C[j - 1];
    }
    size_t prev_count = 0;
    for (size_t i = 0; i <= k; ) {
        size_t count = C[i];
        for (size_t j = prev_count; j <= count; j++) {
            A[j] = i;
        }
        while (C[i] == count) {
            i++;
        }
        prev_count = count;
    }
}

int main() {
    std::vector<size_t> v {2, 5, 3, 0, 2, 3, 0, 3};
    inplaceCountingSort(v, 7);
    for (auto n : v) {
        std::cout << n << ' ';
    }
}