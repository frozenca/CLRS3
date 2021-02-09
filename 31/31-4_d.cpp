#include <bit>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <random>
#include <iostream>

std::size_t ModularExponentiation(std::size_t a, std::size_t b, std::size_t n) {
    assert(n);
    std::size_t c = 0;
    std::size_t d = 1;
    auto k = std::bit_width(b);
    std::bitset<64> B(b);
    for (std::size_t i = k - 1; i < k; i--) {
        c *= 2;
        d = (d * d) % n;
        if (B[i]) {
            c++;
            d = (d * a) % n;
        }
    }
    return d;
}

bool isQuadraticResidue(std::size_t a, std::size_t p) {
    return ModularExponentiation(a, (p - 1) >> 1, p) == 1;
}

std::mt19937 gen(std::random_device{}());

std::size_t findNonQuadraticResidue(std::size_t p) {
    assert(p > 2 && (p & 1));
    std::uniform_int_distribution<> dist(1, p - 1);
    while (true) {
        auto a = dist(gen);
        if (!isQuadraticResidue(a, p)) {
            return a;
        }
    }
}

int main() {
    std::cout << findNonQuadraticResidue(11) << '\n';
    std::cout << findNonQuadraticResidue(11) << '\n';
    std::cout << findNonQuadraticResidue(11) << '\n';
    std::cout << findNonQuadraticResidue(11) << '\n';
    std::cout << findNonQuadraticResidue(11) << '\n';


}