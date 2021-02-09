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
std::pair<std::vector<std::vector<size_t>>, std::vector<std::vector<Dir>>>
LCSLength(const std::vector<T>& X, const std::vector<T>& Y) {
    std::vector<std::vector<Dir>> B (X.size(), std::vector<Dir>(Y.size(), Dir::upleft));
    std::vector<std::vector<size_t>> C (X.size() + 1, std::vector<size_t>(Y.size() + 1));
    for (size_t i = 0; i < X.size(); i++) {
        for (size_t j = 0; j < Y.size(); j++) {
            if (X[i] == Y[j]) {
                C[i + 1][j + 1] = C[i][j] + 1;
                B[i][j] = Dir::upleft;
            } else if (C[i][j + 1] >= C[i + 1][j]) {
                C[i + 1][j + 1] = C[i][j + 1];
                B[i][j] = Dir::up;
            } else {
                C[i + 1][j + 1] = C[i + 1][j];
                B[i][j] = Dir::left;
            }
        }
    }
    return {C, B};
}

template <typename T>
void PrintLCS(const std::vector<std::vector<Dir>>& B, const std::vector<T>& X, const std::vector<T>& Y, size_t i, size_t j) {
    if (i >= X.size() || j >= Y.size()) {
        return;
    }
    if (B[i][j] == Dir::upleft) {
        PrintLCS(B, X, Y, i - 1, j - 1);
        std::cout << X[i];
    } else if (B[i][j] == Dir::up) {
        PrintLCS(B, X, Y, i - 1, j);
    } else {
        PrintLCS(B, X, Y, i, j - 1);
    }
}

int main() {
    std::vector<char> X = {'A', 'B', 'C', 'B', 'D', 'A', 'B'};
    std::vector<char> Y = {'B', 'D', 'C', 'A', 'B', 'A'};
    auto [C, B] = LCSLength(X, Y);
    PrintLCS(B, X, Y, X.size() - 1, Y.size() - 1);


}