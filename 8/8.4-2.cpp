#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>
#include <iostream>
#include <ranges>
#include <functional>

namespace sr = std::ranges;
namespace crn = std::chrono;

template <typename T, typename Comp = std::greater<T>>
void insertionSort(std::vector<T>& A, Comp comp = Comp()) {
    for (size_t j = 1; j < A.size(); j++) {
        T key = A[j];
        size_t i = j - 1;
        while (i < A.size() && comp(A[i], key)) {
            A[i + 1] = A[i];
            i--;
        }
        A[i + 1] = key;
    }
}

void bucketSort(std::vector<double>& A) {
    std::vector<std::vector<double>> B (A.size());
    for (auto d : A) {
        B[std::floor(A.size() * d)].push_back(d);
    }
    for (auto& v : B) {
        sr::sort(v);
    }
    A.clear();
    for (auto& v : B) {
        A.insert(A.end(), std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
    }
}

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    constexpr size_t N = 50'000;
    std::vector<double> A (N);
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
        bucketSort(A);
        auto t4 = crn::steady_clock::now();
        auto dt2 = crn::duration_cast<crn::milliseconds>(t4 - t3);
        DT2 += dt2;
        assert(sr::is_sorted(A));
    }

    std::cout << "Sorting " << N << " elements in [0, 1] using std::sort : " << DT1.count() / TRIALS << "ms\n";
    std::cout << "Sorting " << N << " elements in [0, 1] using bucket sort : " << DT2.count() / TRIALS << "ms\n";


}