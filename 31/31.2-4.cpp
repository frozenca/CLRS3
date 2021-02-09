#include <cstddef>
#include <iostream>
#include <tuple>

std::size_t IterativeEuclid(std::size_t a, std::size_t b) {
    while (b) {
        auto r = a % b;
        std::tie(a, b) = std::tie(b, r);
    }
    return a;
}

int main() {
    std::cout << IterativeEuclid(38, 16);

}