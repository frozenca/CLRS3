#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

int CutRod(const std::vector<int>& p, size_t n) {
    if (!n) {
        return 0;
    }
    int q = std::numeric_limits<int>::lowest();
    for (size_t i = 1; i <= n; i++) {
        q = std::max(q, p[i] + CutRod(p, n - i));
    }
    return q;
}

int MemoizedCutRodAux(const std::vector<int>& p, size_t n, std::vector<int>& r) {
    if (r[n] >= 0) {
        return r[n];
    }
    int& q = r[n];
    if (n == 0) {
        q = 0;
    } else {
        q = std::numeric_limits<int>::lowest();
        for (size_t i = 1; i <= n; i++) {
            q = std::max(q, p[i] + MemoizedCutRodAux(p, n - i, r));
        }
    }
    return q;
}

int MemoizedCutRod(const std::vector<int>& p, size_t n) {
    std::vector<int> r (n + 1, std::numeric_limits<int>::lowest());
    return MemoizedCutRodAux(p, n, r);
}

int BottomUpCutRod(const std::vector<int>& p, size_t n) {
    std::vector<int> r (n + 1);
    for (size_t j = 1; j <= n; j++) {
        int q = std::numeric_limits<int>::lowest();
        for (size_t i = 1; i <= j; i++) {
            q = std::max(q, p[i] + r[j - i]);
        }
        r[j] = q;
    }
    return r[n];
}

std::pair<std::vector<int>, std::vector<size_t>> ExtendedBottomUpCutRod(const std::vector<int>& p, size_t n) {
    std::vector<int> r (n + 1);
    std::vector<size_t> s (n + 1);
    for (size_t j = 1; j <= n; j++) {
        int q = std::numeric_limits<int>::lowest();
        for (size_t i = 1; i <= j; i++) {
            if (q < p[i] + r[j - i]) {
                q = p[i] + r[j - i];
                s[j] = i;
            }
        }
        r[j] = q;
    }
    return {r, s};
}

void PrintCutRodSolution(const std::vector<int>& p, size_t n) {
    auto [r, s] = ExtendedBottomUpCutRod(p, n);
    while (n) {
        std::cout << s[n] << ' ';
        n -= s[n];
    }
    std::cout << '\n';
}

int main() {
    std::vector<int> p {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
    std::cout << CutRod(p, 10) << '\n';
    std::cout << MemoizedCutRod(p, 10) << '\n';
    std::cout << BottomUpCutRod(p, 10) << '\n';
    PrintCutRodSolution(p, 10);

}