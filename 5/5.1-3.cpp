#include <random>
#include <iostream>

std::mt19937 gen(std::random_device{}());
std::bernoulli_distribution BIASED_RANDOM(0.8);

bool UNBIASED_RANDOM() {
    while (true) {
        bool a = BIASED_RANDOM(gen);
        bool b = BIASED_RANDOM(gen);
        if (a != b) {
            return a;
        }
    }
}

int main() {

    size_t count = 0;
    size_t trials = 100000;
    for (size_t i = 0; i < trials; i++) {
        if (UNBIASED_RANDOM()) {
            count++;
        }
    }
    std::cout << static_cast<double>(count) / static_cast<double>(trials) << '\n';
}