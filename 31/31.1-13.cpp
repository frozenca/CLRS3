#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

constexpr std::array<uint64_t, 6> table = {1ull << (1ull << 0ull),
                                           1ull << (1ull << 1ull),
                                              1ull << (1ull << 2ull),
                                              1ull << (1ull << 3ull),
                                              1ull << (1ull << 4ull),
                                              1ull << (1ull << 5ull)};

uint64_t toDecimal(std::vector<bool>& bits) {
    assert(!bits.empty());
    if (bits.size() == 1) {
        return bits[0];
    }
    if (!std::has_single_bit(bits.size())) {
        bits.resize(std::bit_ceil(bits.size()));
    }
    std::size_t n = bits.size();
    std::vector<bool> lower_half (bits.begin(), bits.begin() + n / 2);
    std::vector<bool> upper_half (bits.begin() + n / 2, bits.end());
    return toDecimal(upper_half) * table[std::floor(std::log2(n / 2))] + toDecimal(lower_half);
}

int main() {
    std::vector<bool> bits {false, true, true, false,
                            true, true, false, true,
                            true, true, false, false,
                            true, false, true};
    auto n = toDecimal(bits);
    std::cout << n << '\n';
    std::bitset<16> bit_str (n);
    std::cout << bit_str << '\n';


}