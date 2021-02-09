#include <algorithm>
#include <cassert>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

bool isIndependent(std::vector<size_t>& A) {
    assert(sr::all_of(A, [&A](auto n) {return n < A.size();}));
    size_t n = A.size();
    std::vector<size_t> B (n + 1);
    for (auto k : A) {
        B[k]++;
    }
    for (size_t i = 0; i <= n; i++) {
        B[i + 1] += B[i];
        if (B[i + 1] > i) return false;
    }
    return true;
}

int main() {
}
