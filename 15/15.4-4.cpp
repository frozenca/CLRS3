#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

enum class Dir {
    upleft,
    up,
    left,
};

template <typename T>
int LCSLength(const std::vector<T>& X, const std::vector<T>& Y) {
    size_t m = X.size(), n = Y.size();
    auto X_ = X, Y_ = Y;
    if (m < n) {
        std::swap(X_, Y_);
        std::swap(m, n); // now m > n
    }
    std::vector<int> curr(n + 1);
    int prev = 0;

    for (size_t i = 0; i <= m; i++) {
        prev = curr[0];
        for (size_t j = 0; j <= n; j++) {
            int cache = curr[j];
            if (i == 0 || j == 0) {
                curr[j] = 0;
            } else if (X_[i - 1] == Y_[j - 1]){
                curr[j] = prev + 1;
            } else {
                curr[j] = std::max(curr[j], curr[j - 1]);
            }
            prev = cache;
        }
    }

    return curr[n];
}

int main() {
    std::vector<char> X = {'A', 'B', 'C', 'B', 'D', 'A', 'B'};
    std::vector<char> Y = {'B', 'D', 'C', 'A', 'B', 'A'};
    std::cout << LCSLength(X, Y);


}