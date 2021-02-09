#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

int MemoizedCutRodAux(const std::vector<int>& p, size_t n, std::vector<int>& r, std::vector<size_t>& s) {
    if (r[n] >= 0) {
        return r[n];
    }
    int& q = r[n];
    if (n == 0) {
        q = 0;
    } else {
        q = std::numeric_limits<int>::lowest();
        for (size_t i = 1; i <= n; i++) {
            auto val = MemoizedCutRodAux(p, n - i, r, s);
            if (q < p[i] + val) {
                q = p[i] + val;
                s[n] = i;
            }
        }
    }
    return q;
}

int MemoizedCutRod(const std::vector<int>& p, size_t n) {
    std::vector<int> r (n + 1, std::numeric_limits<int>::lowest());
    std::vector<size_t> s (n + 1);
    auto val = MemoizedCutRodAux(p, n, r, s);
    while (n) {
        std::cout << s[n] << ' ';
        n -= s[n];
    }
    std::cout << '\n';
    return val;
}


int main() {
    std::vector<int> p {0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
    std::cout << MemoizedCutRod(p, 10) << '\n';
}