#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
bool twoSum(std::vector<T>& A, const T& key) {
    std::sort(A.begin(), A.end());
    size_t left = 0, right = A.size() - 1;
    while (left < right) {
        if (A[left] + A[right] < key) {
            left++;
        } else if (A[left] + A[right] > key) {
            right--;
        } else {
            return true;
        }
    }
    return false;
}

int main() {
    std::vector<int> v {1, 2, 4, 6, 7};
    assert(twoSum(v, 11));
    assert(!twoSum(v, 20));
    assert(!twoSum(v, 2));
}