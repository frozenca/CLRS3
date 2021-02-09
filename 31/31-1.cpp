#include <cstddef>
#include <cassert>
#include <iostream>
#include <numeric>

std::size_t BinaryGCD(std::size_t a, std::size_t b) {
    if (a < b) {
        return BinaryGCD(b, a);
    }
    if (a & 1) {
        if (b & 1) {
            if (b == 1) {
                return 1;
            } else {
                return BinaryGCD((a - b) >> 1, b);
            }
        } else {
            if (b == 0) {
                return a;
            } else {
                return BinaryGCD(a, b >> 1);
            }
        }
    } else {
        if (b & 1) {
            if (b == 1) {
                return 1;
            } else {
                return BinaryGCD(a >> 1, b);
            }
        } else {
            if (b == 0) {
                return a;
            } else {
                return BinaryGCD(a >> 1, b >> 1) << 1;
            }
        }
    }
}

int main() {
    std::size_t a = 4;
    std::size_t b = 36;
    std::cout << std::gcd(a, b) << '\n';
    std::cout << BinaryGCD(a, b) << '\n';


}