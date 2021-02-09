#include <algorithm>
#include <cassert>
#include <vector>
#include <cmath>
#include <ranges>

namespace sr = std::ranges;

size_t maximizePayoff(std::vector<size_t>& A, std::vector<size_t>& B) {
    assert(sr::all_of(A, [](auto n) {return n > 0;}));
    assert(sr::all_of(B, [](auto n) {return n > 0;}));
    assert(A.size() == B.size());
    sr::sort(A);
    sr::sort(B);
    size_t prod = 1;
    for (size_t i = 0; i < A.size(); i++) {
        prod *= std::pow(A[i], B[i]);
    }
    return prod;
}

int main() {
}
