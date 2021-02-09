#include <chrono>
#include <cstddef>
#include <iostream>
#include <vector>

namespace crn = std::chrono;

void Increment(std::vector<int>& A) {
    std::size_t i = 0;
    while (i < A.size() - 1 && A[i] == 1) {
        A[i] = 0;
        i++;
    }
    if (i < A.size() - 1) {
        A[i] = 1;
    }
}

int main() {
    constexpr std::size_t SZ = 100'000;
    std::vector<int> v (100'000);
    auto t1 = crn::steady_clock::now();
    for (std::size_t i = 0; i < SZ; i++) {
        Increment(v);
    }
    auto t2 = crn::steady_clock::now();
    auto d1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

}