#include <cstddef>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

namespace crn = std::chrono;

std::size_t Fib(std::size_t n) {
    if (n <= 1) {
        return n;
    } else {
        auto x = Fib(n - 1);
        auto y = Fib(n - 2);
        return x + y;
    }
}

void PFib(std::size_t n, std::size_t& result) {
    if (n <= 1) {
        result += n;
    } else {
        std::jthread t1(PFib, n - 1, std::ref(result));
        PFib(n - 2, result);
    }
}

int main() {
    std::size_t N = 0;

    std::cin >> N;

    auto t1 = crn::steady_clock::now();
    std::cout << Fib(N) << '\n';
    auto t2 = crn::steady_clock::now();
    auto dt1 = crn::duration_cast<crn::microseconds>(t2 - t1);
    std::cout << "Fib : " << dt1.count() << "us\n";
    std::size_t res = 0;
    auto t3 = crn::steady_clock::now();
    PFib(N, res);
    std::cout << res << '\n';
    auto t4 = crn::steady_clock::now();
    auto dt2 = crn::duration_cast<crn::microseconds>(t4 - t3);
    std::cout << "PFib : " << dt2.count() << "us\n";
}