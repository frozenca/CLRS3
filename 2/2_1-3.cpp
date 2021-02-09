#include <cassert>
#include <vector>
#include <iostream>
#include <optional>

template <typename T>
std::optional<size_t> linearSearch(std::vector<T>& A, const T& key) {
    for (size_t i = 0; i < A.size(); i++) {
        if (A[i] == key) {
            return i;
        }
    }
    return {};
}

int main() {
    std::vector<int> v {5, 2, 3, 1, 4};
    assert(linearSearch(v, 3));
    assert(!linearSearch(v, 6));
}