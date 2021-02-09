#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

int ModifiedCutRod(const std::vector<int>& p, size_t n, int c) {
    std::vector<int> r (n + 1);
    for (size_t j = 1; j <= n; j++) {
        int q = p[j];
        for (size_t i = 1; i < j; i++) {
            q = std::max(q, p[i] + r[j - i] - c);
        }
        r[j] = q;
    }
    return r[n];
}

int main() {
    std::vector<int> p {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
    std::cout << ModifiedCutRod(p, 10, 0) << '\n';
    std::cout << ModifiedCutRod(p, 10, 4) << '\n';

}