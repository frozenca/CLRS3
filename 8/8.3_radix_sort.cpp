#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

void radixSort(std::vector<size_t>& A, size_t d) {
    assert(*sr::max_element(A) < std::pow(10u, d));
    for (size_t i = 0; i < d; i++) {
        auto comp = [&i](size_t a, size_t b) {
            return ((a / static_cast<size_t>(std::pow(10u, i))) % 10) <
                   ((b / static_cast<size_t>(std::pow(10u, i))) % 10);
        };
        sr::stable_sort(A, comp);
    }
}

int main() {
    constexpr size_t N = 50'000;
    std::vector<size_t> A (N);
    std::uniform_int_distribution<> dist(0, 99);
    for (auto& n : A) {
        n = dist(gen);
    }
    constexpr size_t TRIALS = 1'000;
    crn::milliseconds DT1 (0), DT2(0);
    for (size_t t = 0; t < TRIALS; t++) {
        sr::shuffle(A, gen);
        auto t1 = crn::steady_clock::now();
        sr::sort(A);
        auto t2 = crn::steady_clock::now();
        auto dt1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
        DT1 += dt1;
        assert(sr::is_sorted(A));
        sr::shuffle(A, gen);
        auto t3 = crn::steady_clock::now();
        radixSort(A, 2);
        auto t4 = crn::steady_clock::now();
        auto dt2 = crn::duration_cast<crn::milliseconds>(t4 - t3);
        DT2 += dt2;
        assert(sr::is_sorted(A));
    }

    std::cout << "Sorting " << N << " elements in range of [0, 99] using std::sort : " << DT1.count() / TRIALS << "ms\n";
    std::cout << "Sorting " << N << " elements in range of [0, 99] using radix sort : " << DT2.count() / TRIALS << "ms\n";


}