#include <cassert>
#include <cmath>
#include <vector>

const double phi = (1.0 + std::sqrt(5.0)) / 2.0;

size_t fibonacciRec(size_t n, std::vector<size_t>& cache) {
    if (!cache[n - 1]) {
        cache[n - 1] = fibonacciRec(n - 1, cache) + fibonacciRec(n - 2, cache);
    }
    return cache[n - 1];
}

size_t fibonacci(size_t n) {
    if (n <= 2) return 1;
    std::vector<size_t> cache(n);
    cache[0] = 1;
    cache[1] = 1;
    return fibonacciRec(n, cache);
}

size_t fibonacci2(size_t n) {
    double phi_n = std::pow(phi, n);
    return static_cast<size_t>(phi_n / std::sqrt(5.0));
}

int main() {
    for (size_t i = 0; i < 30; i++) {
        assert(fibonacci(i) == fibonacci2(i));
    }

}