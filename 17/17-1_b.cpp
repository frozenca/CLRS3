#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <string>

std::size_t bitReversedIncrement(std::size_t i) {
    std::size_t m = 1u << 7u;
    while (m & i) {
        i ^= m;
        m >>= 1u;
    }
    m |= i;
    return m;
}

int main() {
    std::size_t i = 0;
    for (std::size_t j = 0; j < 255; j++) {
        std::cout << i << ' ';
        i = bitReversedIncrement(i);
    }

}