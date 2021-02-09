#include <cassert>
#include <iostream>
#include <vector>

size_t fibonacci(size_t n) {
    if (n <= 1) {
        return 1;
    }
    std::vector<size_t> f (n + 1);
    f[0] = 1;
    f[1] = 1;
    for (size_t i = 2; i <= n; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }
    return f[n];
}

int main() {
    std::cout << fibonacci(23);
}