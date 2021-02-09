#include <bit>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

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

enum class NumberClass {
    Composite,
    Prime,
};

NumberClass Pseudoprime(std::size_t n) {
    if ((ModularExponentiation(2, n - 1, n) % n) != 1) {
        return NumberClass::Composite;
    } else {
        return NumberClass::Prime;
    }
}

bool Witness(std::size_t a, std::size_t n) {
    std::size_t t = 0;
    std::size_t u = n - 1;
    while ((u % 2) == 0) {
        t++;
        u /= 2;
    }
    std::vector<std::size_t> X (t + 1);
    X[0] = ModularExponentiation(a, u, n);
    for (std::size_t i = 1; i <= t; i++) {
        X[i] = (X[i - 1] * X[i - 1]) % n;
        if (X[i] == 1 && X[i - 1] != 1 && X[i - 1] != n - 1) {
            return true;
        }
    }
    if (X[t] != 1) {
        return true;
    }
    return false;
}

std::mt19937 gen(std::random_device{}());

NumberClass MillerRabin(std::size_t n, std::size_t s) {
    std::uniform_int_distribution<> dist(1, n - 1);
    for (std::size_t j = 1; j <= s; j++) {
        std::size_t a = dist(gen);
        if (Witness(a, n)) {
            return NumberClass::Composite;
        }
    }
    return NumberClass::Prime;
}

int main() {


}