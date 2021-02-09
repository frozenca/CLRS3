#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

std::mt19937 gen(std::random_device{}());
std::uniform_int_distribution<> birthday(1, 365);

int main() {
    constexpr size_t N = 94;
    constexpr size_t trials = 1000;
    size_t same_birthday_count = 0;
    for (size_t t = 0; t < trials; t++) {
        std::vector<int> people(N);
        std::unordered_map<int, int> freq;
        for (size_t i = 0; i < N; i++) {
            people[i] = birthday(gen);
            freq[people[i]]++;
        }
        int max_freq = 0;
        for (const auto& [day, ppl] : freq) {
            max_freq = std::max(max_freq, ppl);
        }
        if (max_freq >= 3) {
            same_birthday_count++;
        }
    }

    std::cout << "Probability that at least 3 people have the same birthday with " << N <<
    " peoples : " << static_cast<double>(same_birthday_count) / static_cast<double>(trials) << '\n';


}