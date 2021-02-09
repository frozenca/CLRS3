#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

int main() {
    std::vector<int> v {4, 2, 1, 5, 3, 6};
    sr::nth_element(v, v.begin() + v.size() / 2);
    std::cout << "Smaller elements: ";
    for (size_t i = 0; i < v.size() / 2; i++) {
        std::cout << v[i] << ' ';
    }
    std::cout << "\nLarger elements: ";
    for (size_t i = v.size() / 2 + 1; i < v.size(); i++) {
        std::cout << v[i] << ' ';
    }

}