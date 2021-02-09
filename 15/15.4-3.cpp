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
int MemoizedLCSLengthAux(std::vector<std::vector<int>>& C,
        const std::vector<T>& X, const std::vector<T>& Y, size_t i, size_t j) {
    if (C[i][j] >= 0) {
        return C[i][j];
    }
    int& ret = C[i][j];
    if (i == 0 || j == 0) {
        ret = 0;
        return ret;
    }
    if (X[i] == Y[j]) {
        ret = MemoizedLCSLengthAux(C, X, Y, i - 1, j - 1) + 1;
        return ret;
    }
    ret = std::max(MemoizedLCSLengthAux(C, X, Y, i - 1, j), MemoizedLCSLengthAux(C, X, Y, i, j - 1));
    return ret;
}

template <typename T>
int MemoizedLCSLength(const std::vector<T>& X, const std::vector<T>& Y) {
    std::vector<std::vector<int>> C (X.size() + 1, std::vector<int>(Y.size() + 1, -1));
    MemoizedLCSLengthAux(C, X, Y, X.size(), Y.size());
    return C[X.size()][Y.size()];
}

int main() {
    std::vector<char> X = {'A', 'B', 'C', 'B', 'D', 'A', 'B'};
    std::vector<char> Y = {'B', 'D', 'C', 'A', 'B', 'A'};
    std::cout << MemoizedLCSLength(X, Y);


}