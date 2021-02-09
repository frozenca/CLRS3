#include <algorithm>
#include <chrono>
#include <execution>
#include <numeric>
#include <iostream>
#include <random>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>

namespace se = std::execution;
namespace crn = std::chrono;
namespace sr = std::ranges;

template <typename T>
void PMerge(std::vector<T>& B, std::size_t p1, std::size_t r1, std::size_t p2, std::size_t r2,
            std::vector<T>& A, std::size_t p3) {
    std::size_t n1 = r1 - p1 + 1;
    std::size_t n2 = r2 - p2 + 1;
    if (n1 < n2) {
        std::swap(p1, p2);
        std::swap(r1, r2);
        std::swap(n1, n2);
    }
    if (n1 == 0) {
        return;
    } else {
        std::size_t q1 = (p1 + r1) / 2;
        std::size_t q2 = std::distance(B.begin(), std::lower_bound(B.begin() + p2, B.begin() + r2 + 1, B[q1]));
        std::size_t q3 = p3 + (q1 - p1) + (q2 - p2);
        A[q3] = B[q1];
        std::jthread th (&PMerge<T>, std::ref(B), p1, q1 - 1, p2, q2 - 1, std::ref(A), p3);
        PMerge(B, q1 + 1, r1, q2, r2, A, q3 + 1);
    }
}

template <typename T>
void PMergeSort(std::vector<T>& A, std::size_t p, std::size_t r,
                std::vector<T>& B, std::size_t s) {
    std::size_t n = r - p + 1;
    if (n == 1) {
        B[s] = A[p];
    } else {
        std::vector<T> C (n);
        std::size_t q = (p + r) / 2;
        std::size_t q2 = q - p + 1;
        {
            std::jthread th(&PMergeSort<T>, std::ref(A), p, q, std::ref(C), 0);
            PMergeSort(A, q + 1, r, C, q2);
        }
        PMerge(C, 0, q2 - 1, q2, n - 1, B, s);
    }
}


int main() {
    constexpr std::size_t N = 2'000;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle (v, gen);
    auto v2 = v;
    auto v3 = v;
    auto v4 = v;
    auto v5 = v;
    std::vector<int> B (N);
    auto t1 = crn::steady_clock::now();
    PMergeSort(v, 0, N - 1, B, 0);
    auto t2 = crn::steady_clock::now();
    auto dt1 = crn::duration_cast<crn::microseconds>(t2 - t1);
    std::cout << "Aggressively parallelized merge sort : " << dt1.count() << "us\n";
    auto t3 = crn::steady_clock::now();
    sr::sort(v2);
    auto t4 = crn::steady_clock::now();
    auto dt2 = crn::duration_cast<crn::microseconds>(t4 - t3);
    std::cout << "std::sort : " << dt2.count() << "us\n";
    auto t5 = crn::steady_clock::now();
    std::sort(se::par_unseq, v3.begin(), v3.end());
    auto t6 = crn::steady_clock::now();
    auto dt3 = crn::duration_cast<crn::microseconds>(t6 - t5);
    std::cout << "std::sort (par_unseq) : " << dt3.count() << "us\n";
    auto t7 = crn::steady_clock::now();
    std::sort(se::unseq, v4.begin(), v4.end());
    auto t8 = crn::steady_clock::now();
    auto dt4 = crn::duration_cast<crn::microseconds>(t8 - t7);
    std::cout << "std::sort (unseq) : " << dt4.count() << "us\n";
    auto t9 = crn::steady_clock::now();
    std::sort(se::par, v5.begin(), v5.end());
    auto t10 = crn::steady_clock::now();
    auto dt5 = crn::duration_cast<crn::microseconds>(t10 - t9);
    std::cout << "std::sort (par) : " << dt5.count() << "us\n";


}
