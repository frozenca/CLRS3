#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());
std::uniform_int_distribution<> birthday(1, 365);

int main() {
    constexpr size_t N = 23;
    constexpr size_t trials = 1000;
    size_t same_birthday_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::vector<int> people(N);
        for (size_t i = 0; i < N; i++) {
            people[i] = birthday(gen);
        }
        sr::sort(people);
        people.erase(sr::unique(people), people.end());
        if (people.size() < N) {
            same_birthday_count++;
        }
    }

    std::cout << "Probability that at least 2 people have the same birthday : " << static_cast<double>(same_birthday_count) /
            static_cast<double>(trials) << '\n';
    std::cout << "1 - exp(-k(k-1)/2n) : " << 1.0 - std::exp(-static_cast<double>(N) * (N - 1) / (2.0 * 365));


}