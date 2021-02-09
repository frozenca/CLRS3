#include <cstddef>
#include <iostream>
#include <numeric>
#include <vector>

std::size_t LCM(const std::vector<std::size_t>& A) {
    if (A.empty()) {
        return 1;
    }
    auto x = A[0];
    auto g = A[0];
    for (std::size_t i = 1; i < A.size(); i++) {
        x *= A[i];
        g = std::gcd(g, A[i]);
    }
    return x / g;
}

int main() {
    std::vector<std::size_t> v {6, 7, 20, 14, 9};
    std::cout << LCM(v);

}