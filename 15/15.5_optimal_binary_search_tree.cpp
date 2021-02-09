#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

std::pair<std::vector<std::vector<double>>, std::vector<std::vector<size_t>>>
OptimalBST(const std::vector<double>& p, const std::vector<double>& q) {
    size_t n = p.size();
    std::vector<std::vector<double>> expect (n + 2, std::vector<double>(n + 1));
    std::vector<std::vector<double>> partialProbSum (n + 2, std::vector<double>(n + 1));
    std::vector<std::vector<size_t>> root (n + 1, std::vector<size_t>(n + 1));
    for (size_t i = 1; i <= n + 1; i++) {
        expect[i][i - 1] = q[i];
        partialProbSum[i][i - 1] = q[i];
    }
    for (size_t l = 1; l <= n; l++) { // l : length of chain
        for (size_t i = 1; i <= n - l + 1; i++) { // begin
            size_t j = i + l - 1; // end
            expect[i][j] = std::numeric_limits<double>::max();
            partialProbSum[i][j] = partialProbSum[i][j - 1] + p[j] + q[j];
            for (size_t r = i; r <= j; r++) { // root
                double v = expect[i][r - 1] + expect[r + 1][j] + partialProbSum[i][j];
                if (v < expect[i][j]) {
                    expect[i][j] = v;
                    root[i][j] = r;
                }
            }
        }
    }
    return {expect, root};
}

int main() {

}