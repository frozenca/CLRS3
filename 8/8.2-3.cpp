#include <algorithm>
#include <cassert>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::vector<size_t> countingSort(std::vector<size_t>& A, size_t k) {
    assert(*sr::max_element(A) <= k);
    std::vector<size_t> B (A.size());
    std::vector<size_t> C (k + 1);
    for (auto n : A) {
        C[n]++;
    }
    for (size_t j = 1; j <= k; j++) {
        C[j] += C[j - 1];
    }
    for (auto n : A) {
        B[C[n]] = n;
        C[n]--;
    }
    return B;
}

int main() {
    std::vector<size_t> v {3, 2, 6, 1, 5, 4};
    v = countingSort(v, 6);
    assert(sr::is_sorted(v));
}