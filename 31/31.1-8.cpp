#include <bitset>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cmath>
#include <iostream>

std::strong_ordering isKthPowerEqualTo(std::size_t a, std::size_t k, std::size_t n) {
    std::size_t d = 1;
    auto bits = static_cast<std::size_t>(std::ceil(std::log2(k)));
    std::bitset<64> K(k);
    for (std::size_t i = bits; i <= bits; i--) {
        d *= d;
        if (d > n) {
            return std::strong_ordering::greater;
        }
        if (K[i] == 1) {
            d *= a;
            if (d > n) {
                return std::strong_ordering::greater;
            }
        }
    }
    return d <=> n;
}

bool isKthPower(std::size_t n, std::size_t k) {
    assert(n > 1 && k > 1);
    std::size_t l = 2;
    std::size_t r = n / 2;
    while (l <= r) {
        std::size_t m = (l + r) / 2;
        auto res = isKthPowerEqualTo(m, k, n);
        if (res == std::strong_ordering::greater) {
            r = m - 1;
        } else if (res == std::strong_ordering::equal) {
            return true;
        } else {
            l = m + 1;
        }
    }
    return false;
}

bool isNontrivialPower(std::size_t n) {
    assert(n > 1);
    auto b = static_cast<std::size_t>(std::ceil(std::log2(n)));
    for (std::size_t k = 2; k <= b; k++) {
        if (isKthPower(n, k)) {
            return true;
        }
    }
    return false;
}

int main() {
    assert(isNontrivialPower(19487171));

}