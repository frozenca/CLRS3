#include <cassert>
#include <tuple>

std::tuple<size_t, size_t, size_t, size_t> coinChange(size_t n) {
    auto q = n / 25;
    n -= q * 25;
    auto d = n / 10;
    n -= d * 10;
    auto k = n / 5;
    n -= k * 5;
    auto p = n;
    return {q, d, k, p};
}

int main() {
}
