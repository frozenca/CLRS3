#include <bit>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>

std::pair<std::size_t, std::size_t> Division(std::size_t a, std::size_t b) {
    assert(a > b && b > 0);
    std::size_t p = 0;
    while (a >= b) {
        auto w_diff = std::bit_width(a) - std::bit_width(b);
        a -= (b << w_diff);
        p += (1 << w_diff);
    }
    return {p, a};
}

int main() {
    auto a = 1373267;
    auto b = 2293;
    auto [p, q] = Division(a, b);
    std::cout << p << ' ' << q << '\n';
    std::cout << a / b << ' ' << a % b << '\n';


}