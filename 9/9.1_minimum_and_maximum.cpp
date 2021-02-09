#include <cassert>
#include <vector>

template <typename T>
T minimum(const std::vector<T>& A) {
    T minValue = A[0];
    for (size_t i = 1; i < A.size(); i++) {
        if (minValue > A[i]) {
            minValue = A[i];
        }
    }
    return minValue;
}

template <typename T>
T maximum(const std::vector<T>& A) {
    T maxValue = A[0];
    for (size_t i = 1; i < A.size(); i++) {
        if (maxValue < A[i]) {
            maxValue = A[i];
        }
    }
    return maxValue;
}

int main() {

}