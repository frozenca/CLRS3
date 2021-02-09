#include <cmath>
#include <iostream>
#include <random>

std::mt19937 gen(std::random_device{}());

int main() {
    constexpr size_t trials = 1000;
    size_t sum = 0;
    std::uniform_int_distribution<> dice(1, 6);

    for (size_t i = 0; i < trials; i++) {
        sum += dice(gen);
    }

    std::cout << "sum / trials of a dice : " << static_cast<double>(sum) / static_cast<double>(trials) << '\n';

}