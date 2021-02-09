#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

enum class Op {
    Copy,
    Replace,
    Delete,
    Insert,
    Twiddle,
    Kill,
};

constexpr double COST_COPY = 1.0;
constexpr double COST_REPLACE = 1.3;
constexpr double COST_DELETE = 1.6;
constexpr double COST_INSERT = 1.9;
constexpr double COST_TWIDDLE = 2.2;
constexpr double COST_KILL = 2.5;

double EditDistance(const std::string& x, const std::string& y) {
    size_t m = x.length();
    size_t n = x.length();

    std::vector<std::vector<double>> c (m + 1, std::vector<double> (n + 1));
    for (size_t i = 0; i <= m; i++) {
        c[i][0] = i * COST_DELETE;
    }
    for (size_t j = 0; j <= n; j++) {
        c[0][j] = j * COST_INSERT;
    }
    for (size_t i = 1; i <= m; i++) {
        for (size_t j = 1; j <= n; j++) {
            c[i][j] = std::numeric_limits<double>::max();
            if (x[i - 1] == y[j - 1]) {
                c[i][j] = c[i - 1][j - 1] + COST_COPY;
            }
            if (x[i - 1] != y[j - 1] && c[i - 1][j - 1] + COST_REPLACE < c[i][j]) {
                c[i][j] = c[i - 1][j - 1] + COST_REPLACE;
            }
            if (i >= 2 && j >= 2 && x[i - 1] == y[j - 2] && x[i - 2] == y[j - 1] &&
                c[i - 2][j - 2] + COST_TWIDDLE < c[i][j]) {
                c[i][j] = c[i - 2][j - 2] + COST_TWIDDLE;
            }
            if (c[i - 1][j] + COST_DELETE < c[i][j]) {
                c[i][j] = c[i - 1][j] + COST_DELETE;
            }
            if (c[i][j - 1] + COST_INSERT < c[i][j]) {
                c[i][j] = c[i][j - 1] + COST_INSERT;
            }
        }
    }
    for (size_t i = 0; i < m - 1; i++) {
        if (c[i][n] + COST_KILL < c[m][n]) {
            c[m][n] = c[i][n] + COST_KILL;
        }
    }
    return c[0][n];
}

int main() {
    std::cout << EditDistance("algorithm", "altruistic");

}