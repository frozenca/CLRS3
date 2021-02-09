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
        expect[i][i - 1] = q[i - 1];
        partialProbSum[i][i - 1] = q[i - 1];
    }
    for (size_t l = 1; l <= n; l++) { // l : length of chain
        for (size_t i = 1; i <= n - l + 1; i++) { // begin
            if (l == 1) {
                size_t j = i + l - 1; // end
                expect[i][j] = std::numeric_limits<double>::max();
                partialProbSum[i][j] = partialProbSum[i][j - 1] + p[j - 1] + q[j];
                for (size_t r = i; r <= j; r++) { // root
                    double v = expect[i][r - 1] + expect[r + 1][j] + partialProbSum[i][j];
                    if (v < expect[i][j]) {
                        expect[i][j] = v;
                        root[i][j] = r;
                    }
                }
            } else {
                size_t j = i + l - 1; // end
                expect[i][j] = std::numeric_limits<double>::max();
                partialProbSum[i][j] = partialProbSum[i][j - 1] + p[j - 1] + q[j];
                for (size_t r = root[i][j - 1]; r <= root[i + 1][j]; r++) { // root
                    double v = expect[i][r - 1] + expect[r + 1][j] + partialProbSum[i][j];
                    if (v < expect[i][j]) {
                        expect[i][j] = v;
                        root[i][j] = r;
                    }
                }
            }
        }
    }
    return {expect, root};
}

void ConstructOptimalBst(const std::vector<std::vector<size_t>>& root,
        size_t i, size_t j, size_t last, size_t& d_index) {
    if (i > j) {
        if (j < last) {
            std::cout << "d_" << d_index++ << " is the left child of k_" << last << "\n";
        } else {
            std::cout << "d_" << d_index++ << " is the right child of k_" << last << "\n";
        }
        return;
    }
    if (last == 0) {
        std::cout << "k_" << root[i][j] << " is the root\n";
    } else if (j < last) {
        std::cout << "k_" << root[i][j] << " is the left child of k_" << last << "\n";
    } else {
        std::cout << "k_" << root[i][j] << " is the right child of k_" << last << "\n";
    }
    ConstructOptimalBst(root, i, root[i][j] - 1, root[i][j], d_index);
    ConstructOptimalBst(root, root[i][j] + 1, j, root[i][j], d_index);
}

int main() {
    std::vector<double> p {0.15, 0.10, 0.05, 0.10, 0.20};
    std::vector<double> q {0.05, 0.10, 0.05, 0.05, 0.05, 0.10};
    auto [expect, root] = OptimalBST(p, q);
    size_t d_index = 0;
    ConstructOptimalBst(root, 1, p.size(), 0, d_index);

}