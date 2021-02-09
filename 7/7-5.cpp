#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <numeric>
#include <vector>
#include <ranges>

namespace sr = std::ranges;
namespace crn = std::chrono;

std::mt19937 gen(std::random_device{}());

template <typename T>
size_t partition(std::vector<T>& A, size_t p, size_t r) {
    T x = A[r];
    size_t i = p;
    for (size_t j = p; j < r; j++) {
        if (A[j] <= x) {
            std::swap(A[i], A[j]);
            i++;
        }
    }
    std::swap(A[i], A[r]);
    return i;
}

template <typename T>
size_t medianThreePartition(std::vector<T>& A, size_t p, size_t r) {
    std::uniform_int_distribution<> dist(p, r);
    std::vector<size_t> temp;
    temp.push_back(dist(gen));
    temp.push_back(dist(gen));
    temp.push_back(dist(gen));
    std::sort(temp.begin(), temp.end());
    size_t idx = temp[1];

    T x = A[idx];
    size_t i = p;
    for (size_t j = p; j < r; j++) {
        if (A[j] >= x) {
            std::swap(A[i], A[j]);
            i++;
        }
    }
    std::swap(A[i], A[r]);
    return i;
}

template <typename T>
void medianThreeQuickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        size_t q = medianThreePartition(A, p, r);
        medianThreeQuickSort(A, p, q - 1);
        medianThreeQuickSort(A, q + 1, r);
    }
}

template <typename T>
void quickSort(std::vector<T>& A, size_t p, size_t r) {
    if (p < r && r < A.size()) {
        size_t q = partition(A, p, r);
        quickSort(A, p, q - 1);
        quickSort(A, q + 1, r);
    }
}

int main() {
    constexpr size_t N = 10'000;
    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);

    constexpr size_t TRIALS = 1'000;

    crn::microseconds DT(0), DT2(0), DT3(0);
    for (size_t t = 0; t < TRIALS; t++) {
        auto u = v;
        sr::shuffle(u, gen);
        auto t1 = crn::steady_clock::now();
        medianThreeQuickSort(u, 0, u.size() - 1);
        auto t2 = crn::steady_clock::now();
        auto dt = crn::duration_cast<crn::microseconds>(t2 - t1);
        DT += dt;

        sr::shuffle(u, gen);
        auto t3 = crn::steady_clock::now();
        quickSort(u, 0, u.size() - 1);
        auto t4 = crn::steady_clock::now();
        auto dt2 = crn::duration_cast<crn::microseconds>(t4 - t3);
        DT2 += dt2;

        sr::shuffle(u, gen);
        auto t5 = crn::steady_clock::now();
        sr::sort(u);
        auto t6 = crn::steady_clock::now();
        auto dt3 = crn::duration_cast<crn::microseconds>(t6 - t5);

        DT3 += dt3;
    }
    std::cout << "Average performance of median-of-3 quicksort on " << N << " elements : " << DT.count() / TRIALS << "ms\n";
    std::cout << "Average performance of quicksort on " << N << " elements : " << DT2.count() / TRIALS << "ms\n";
    std::cout << "Average performance of std::sort on " << N << " elements : " << DT3.count() / TRIALS << "ms\n";

}