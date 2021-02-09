#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

enum class Dir {
    upleft,
    up,
    upright,
};

std::vector<std::pair<size_t, size_t>> Seam(const std::vector<std::vector<double>>& d) {
    size_t m = d.size();
    size_t n = d[0].size();

    std::vector<std::vector<double>> D (m, std::vector<double>(n));
    std::vector<std::vector<Dir>> S (m, std::vector<Dir>(n, Dir::up));
    for (size_t i = 0; i < n; i++) {
        D[0][i] = d[0][i];
    }
    for (size_t i = 1; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            if (j == 0) {
                if (D[i - 1][j] < D[i - 1][j + 1]) {
                    D[i][j] = D[i - 1][j] + d[i][j];
                    S[i][j] = Dir::up;
                } else {
                    D[i][j] = D[i - 1][j + 1] + d[i][j];
                    S[i][j] = Dir::upright;
                }
            } else if (j == n - 1) {
                if (D[i - 1][j - 1] < D[i - 1][j]) {
                    D[i][j] = D[i - 1][j - 1] + d[i][j];
                    S[i][j] = Dir::upleft;
                } else {
                    D[i][j] = D[i - 1][j] + d[i][j];
                    S[i][j] = Dir::up;
                }
            } else {
                auto x = std::min({D[i - 1][j - 1], D[i - 1][j], D[i - 1][j + 1]});
                D[i][j] = x + d[i][j];
                if (x == D[i - 1][j - 1]) {
                    S[i][j] = Dir::upleft;
                } else if (x == D[i - 1][j]) {
                    S[i][j] = Dir::up;
                } else {
                    S[i][j] = Dir::upright;
                }
            }
        }
    }
    size_t q = 0;
    for (size_t j = 0; j < n; j++) {
        if (D[m][j] < D[m][q]) {
            q = j;
        }
    }
    size_t x = m, y = q;
    std::vector<std::pair<size_t, size_t>> pos;
    while (x) {
        pos.emplace_back(x, y);
        if (S[x][y] == Dir::upleft) {
            x--;
            y--;
        } else if (S[x][y] == Dir::up) {
            x--;
        } else {
            x--;
            y++;
        }
    }
    sr::reverse(pos);
    return pos;
}



int main() {


}