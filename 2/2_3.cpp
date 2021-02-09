#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
T horner(const std::vector<T>& A, T x) {
    T value = 0;
    for (size_t i = 0; i < A.size(); i++) {
        value = A[i] + x * value;
    }
    return value;
}

int main() {
    std::vector<double> coefficients {1, -2, 1};
    std::cout << horner(coefficients, 1.0) << '\n';
    std::cout << horner(coefficients, 2.0) << '\n';
}