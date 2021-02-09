#include <cstddef>
#include <cassert>
#include <tuple>

std::size_t Euclid(std::size_t a, std::size_t b) {
    if (!b) {
        return a;
    } else {
        return Euclid(b, a % b);
    }
}

std::tuple<std::size_t, int, int> ExtendedEuclid(std::size_t a, std::size_t b) {
    if (!b) {
        return {a, 1, 0};
    } else {
        auto [d_, x_, y_] = ExtendedEuclid(b, a % b);
        std::size_t d = d_;
        int x = y_;
        int y = x_ - static_cast<int>(a / b) * y_;
        return {d, x, y};
    }
}

int main() {
    assert(Euclid(30, 21) == 3);

}