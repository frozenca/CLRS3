#include <cassert>
#include <vector>
#include <queue>
#include <functional>

template <typename T>
std::priority_queue<T> HuffmanTernary(const std::vector<T>& C) {
    assert(!C.empty());
    size_t n = C.size();
    std::priority_queue<T, std::vector<T>, std::greater<T>> Q (C.begin(), C.end());
    for (size_t i = 0; i < (n - 1) / 2; i++) {
        auto x = Q.top();
        Q.pop();
        auto y = Q.top();
        Q.pop();
        auto z = Q.top();
        Q.pop();
        auto w = x + y + z;
        Q.push(w);
    }
    return Q;
}

int main() {
}
