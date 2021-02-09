#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

using Player = std::pair<size_t, double>; // (cost, vorp)

double Baseball(size_t N, size_t X, const std::vector<std::vector<Player>>& players) {
    std::vector<std::vector<double>> f (N + 1, std::vector<double>(X + 1));
    for (size_t i = 1; i <= N; i++) {
        for (size_t j = 0; j <= X; j++) {
            f[i][j] = f[i - 1][j]; // don't sign
            for (const auto& [cost, vorp] : players[i]) {
                if (j >= cost) {
                    f[i][j] = std::max(f[i][j], f[i - 1][j - cost] + vorp); // sign
                }
            }
        }
    }
    return f[N][X];
}

int main() {

}