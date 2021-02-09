#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>
#include <numeric>
#include <utility>
#include <string>
#include <ranges>

namespace sr = std::ranges;

template <typename T>
void bitReversalPermutation(std::vector<T>& v) {
    assert(v.size() > 128 && v.size() <= 256);
    std::vector<int> checked (v.size());
    for (std::size_t i = 0; i < v.size(); i++) {
        if (!checked[i]) {
            std::bitset<8> bits (i);
            std::string str = bits.to_string();
            sr::reverse(str);
            std::bitset<8> rev_bits (str);
            std::size_t j = rev_bits.to_ulong();
            std::swap(v[i], v[j]);
            checked[i]++;
            checked[j]++;
        }
    }
}

int main() {
    std::vector<int> v (256);
    std::iota(v.begin(), v.end(), 0);
    bitReversalPermutation(v);
    for (auto n : v) {
        std::cout << n << ' ';
    }

}