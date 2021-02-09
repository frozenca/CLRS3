#include <chrono>
#include <cstddef>
#include <iostream>
#include <vector>
#include <random>

namespace crn = std::chrono;

int main() {
    constexpr std::size_t SIZE = 100'000;
    std::mt19937 gen(std::random_device{}());
    std::bernoulli_distribution dist(0.5);

    std::vector<int> v;

    auto t1 = crn::steady_clock::now();
    for (std::size_t i = 0; i < SIZE; i++) {
        v.push_back(i);
    }
    auto t2 = crn::steady_clock::now();
    auto d1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";
    t1 = crn::steady_clock::now();
    for (std::size_t i = 0; i < SIZE; i++) {
        v.pop_back();
    }
    t2 = crn::steady_clock::now();
    d1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";
    t1 = crn::steady_clock::now();
    for (std::size_t i = 0; i < SIZE; i++) {
        if (dist(gen) || v.empty()) {
            v.push_back(i);
        } else {
            v.pop_back();
        }
    }
    t2 = crn::steady_clock::now();
    d1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

}