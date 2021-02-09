#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <ranges>

std::mt19937 gen(std::random_device{}());

namespace sr = std::ranges;

int main() {
    constexpr size_t N = 100;
    constexpr size_t trials = 1000;
    std::uniform_int_distribution<> toss(0, N - 1);
    size_t toss_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::vector<int> bin(N);
        while (true) {
            bin[toss(gen)] = 1;
            toss_count++;
            if (sr::all_of(bin, [](int i){return i;})) {
                break;
            }
        }
    }

    std::cout << "Expected number of tosses : " << static_cast<double>(toss_count) /
            static_cast<double>(trials) << '\n';
    std::cout << "N ln N : " << N * std::log(N);

}