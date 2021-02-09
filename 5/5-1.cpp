#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

std::mt19937 gen(std::random_device{}());

int main() {
    constexpr size_t N = 100;
    constexpr size_t trials = 1000;
    std::uniform_int_distribution<> incr(1, N);

    size_t total_num = 0;
    for (size_t t = 0; t < trials; t++) {
        std::vector<size_t> seq(N);
        size_t cur_num = 0;
        for (size_t i = 0; i < N; i++) {
            cur_num += incr(gen);
            seq[i] = cur_num;
        }
        cur_num = 0;
        size_t cur_index = 0;
        for (size_t i = 0; i < N; i++) {
            std::bernoulli_distribution cur_incr(1.0 / static_cast<double>(seq[cur_index + 1] - seq[cur_index]));
            if (cur_incr(gen)) {
                cur_num += seq[cur_index + 1] - seq[cur_index];
                cur_index++;
            }
        }
        total_num += cur_num;
    }
    std::cout << "Expected value of counter after N increment operations : " << static_cast<double>(total_num) / static_cast<double>(trials);


}