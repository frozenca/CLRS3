#include <cmath>
#include <random>
#include <iostream>

std::mt19937 gen(std::random_device{}());
std::bernoulli_distribution dist(0.5);

int RANDOM(int a, int b) {
    if (a > b) {
        return RANDOM(b, a);
    }
    if (a == b) {
        return a;
    }
    double m = a + (b - a) / 2.0;
    int r = dist(gen);
    if (r) {
        return RANDOM(std::ceil(m), b);
    } else {
        return RANDOM(a, std::floor(m));
    }
}

int main() {
    for (size_t i = 0; i < 30; i++) {
        std::cout << RANDOM(8, 13) << ' ';
    }

}