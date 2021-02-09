#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <ranges>

namespace sr = std::ranges;
namespace crn = std::chrono;

std::mt19937 gen(std::random_device{}());

void radixSort(std::vector<size_t>& A, size_t d) {
    assert(*sr::max_element(A) < std::pow(10u, d));
    for (size_t i = 0; i < d; i++) {
        auto comp = [&i](size_t a, size_t b) {
            return ((a / static_cast<size_t>(std::pow(10u, i))) % 10) <
                   ((b / static_cast<size_t>(std::pow(10u, i))) % 10);
        };
        sr::sort(A, comp);
    }
}

constexpr size_t N = 3;

void bucketSort(std::vector<size_t>& A) {
    std::vector<std::vector<size_t>> B (N);
    for (auto d : A) {
        B[std::floor(std::log10(d))].push_back(d);
    }
    for (size_t i = 0; i < N; i++) {
        radixSort(B[i], i);
    }
    A.clear();
    for (auto& v : B) {
        A.insert(A.end(), std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
    }
}

int main() {
    constexpr size_t ELEMS = 100'000;
    std::vector<size_t> A (ELEMS);
    std::uniform_int_distribution<> dist(0, std::pow(10, N) - 1);
    for (auto& n : A) {
        n = dist(gen);
    }
    constexpr size_t TRIALS = 200;
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
        bucketSort(A);
        auto t4 = crn::steady_clock::now();
        auto dt2 = crn::duration_cast<crn::milliseconds>(t4 - t3);
        DT2 += dt2;
        assert(sr::is_sorted(A));
    }

    std::cout << "Sorting " << ELEMS << " elements in [0, 10^3 - 1] using std::sort : " << DT1.count() / TRIALS << "ms\n";
    std::cout << "Sorting " << ELEMS << " elements in [0, 10^3 - 1] using bucket + radix sort : " << DT2.count() / TRIALS << "ms\n";


}