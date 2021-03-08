#include <cstddef>
#include <iostream>
#include <utility>

std::pair<std::size_t, std::size_t> DivideEvenly(std::size_t n_x, std::size_t x,
                                                 std::size_t n_y, std::size_t y) {
    std::size_t total = n_x * x + n_y * y;
    if (total % 2 == 1) {
        return {0, 0};
    }
    std::size_t half = total / 2;
    for (std::size_t i = 1; i <= n_x; i++) {
        for (std::size_t j = 1; j <= n_y; j++) {
            if (i * x + j * y == half) {
                return {i, j};
            }
        }
    }
    return {0, 0};
}

int main() {
    auto [b_x, b_y] = DivideEvenly(4, 7, 6, 13);
    std::cout << b_x << ' ' << b_y << '\n';

}