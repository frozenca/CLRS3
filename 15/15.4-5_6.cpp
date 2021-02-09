#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
std::vector<T> LongestIncreasingSubsequence(const std::vector<T>& X) {
    assert(!X.empty());
    std::vector<std::vector<T>> LIS;
    std::vector<T> endpoints;
    LIS.push_back(std::vector<T>({X[0]}));
    endpoints.push_back(X[0]);

    for (size_t i = 1; i < X.size(); i++) {
        auto insertionPoint = sr::lower_bound(endpoints, X[i]);
        if (insertionPoint == endpoints.end()) {
            LIS.push_back(LIS.back());
            LIS.back().push_back(X[i]);
            endpoints.push_back(X[i]);
        } else {
            *insertionPoint = X[i];
            int index = std::distance(endpoints.begin(), insertionPoint);
            LIS[index].back() = X[i];
        }
    }
    return LIS.back();
}

int main() {
    std::vector<int> v {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
    auto LIS = LongestIncreasingSubsequence(v);
    for (auto n : LIS) {
        std::cout << n << ' ';
    }

}