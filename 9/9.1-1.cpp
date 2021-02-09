#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
T secondSmallest(const std::vector<T>& A) {
    assert(T.size() >= 2);
    std::vector<T> B = A;
    std::vector<std::pair<T, T>> total_results;
    std::vector<T> C;
    while (B.size() > 1) {
        C.clear();
        for (size_t i = 0; i < B.size(); i += 2) {
            if (i == B.size() - 1) {
                C.push_back(B[i]);
                total_results.emplace_back(B[i], B[i]);
            } else if (B[i] < B[i + 1]) {
                C.push_back(B[i]);
                total_results.emplace_back(B[i], B[i + 1]);
            } else { // B[i] >= B[i + 1]
                C.push_back(B[i + 1]);
                total_results.emplace_back(B[i + 1], B[i]);
            }
        }
        B = C;
    }
    T minElement = C[0];
    C.clear();
    for (const auto& [small, big] : total_results) {
        if (small == minElement) {
            C.push_back(big);
        }
    }
    return *sr::min_element(C);
}

int main() {
    std::vector<int> v{1, 5, 3, 2, 6};
    std::cout << secondSmallest(v);

}