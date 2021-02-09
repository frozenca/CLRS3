#include <algorithm>
#include <cassert>
#include <chrono>
#include <execution>
#include <numeric>
#include <iostream>
#include <random>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>
#include <optional>

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

template <typename T>
std::size_t Median(std::vector<T>& A, std::size_t p1, std::size_t r1,
                   std::size_t p2, std::size_t r2) {
    if (p1 > r1 && p2 <= r2) {
        return (p2 + r2) / 2;
    } else if (p2 > r2 && p1 <= r1) {
        return (p1 + r1) / 2;
    }
    if (r1 - p1 + 1 > r2 - p2 + 1) {
        std::swap(r1, r2);
        std::swap(p1, p2);
    }
    std::size_t m = r1 - p1 + 1;
    std::size_t n = r2 - p2 + 1;
    std::size_t k = (m + n + 1) / 2;

    std::size_t i_min = 0, i_max = m;

    while (i_min <= i_max) {
        std::size_t i = (i_max + i_min) / 2;
        std::size_t j = k - i;
        if (i < m && A[p2 + j - 1] > A[p1 + i]) {
            i_min = i + 1;
        } else if (i > 0 && A[p2 + j] < A[p1 + i - 1]) {
            i_max = i;
        } else if (i == 0 || A[p2 + j - 1] > A[p1 + i - 1]) {
            return p2 + j - 1;
        } else if (j == 0 || A[p1 + i - 1] >= A[p2 + j - 1]) {
            return p1 + i - 1;
        }
    }
    return -1;
}

template <typename T>
void PMedianMerge(std::vector<T>& B, std::size_t p1, std::size_t r1, std::size_t p2, std::size_t r2,
            std::vector<T>& A, std::size_t p3) {
    std::size_t n1 = r1 - p1 + 1;
    std::size_t n2 = r2 - p2 + 1;
    if ((n1 + 1 <= 1 || r1 + 1 < r1) && (n2 + 1 <= 1 || r2 + 1 < r2)) {
        return;
    }
    std::size_t q1 = Median(B, p1, r1, p2, r2);
    if (q1 >= p1 && q1 <= r1 && r1 + 1 >= r1) {
        std::size_t q2 = std::distance(B.begin(), std::lower_bound(B.begin() + p2, B.begin() + r2 + 1, B[q1]));
        std::size_t q3 = p3 + (q1 - p1) + (q2 - p2);
        A[q3] = B[q1];
        std::jthread t (&PMedianMerge<T>, std::ref(B), p1, q1 - 1, p2, q2 - 1, std::ref(A), p3);
        PMedianMerge(B, q1 + 1, r1, q2, r2, A, q3 + 1);
    } else {
        std::size_t q2 = std::distance(B.begin(), std::lower_bound(B.begin() + p1, B.begin() + r1 + 1, B[q1]));
        std::size_t q3 = p3 + (q1 - p1) + (q2 - p2);
        A[q3] = B[q1];
        std::jthread t (&PMedianMerge<T>, std::ref(B), p1, q2 - 1, p2, q1 - 1, std::ref(A), p3);
        PMedianMerge(B, q2, r1, q1 + 1, r2, A, q3 + 1);
    }
}

template <typename T>
void PMedianMergeSort(std::vector<T>& A, std::size_t p, std::size_t r,
                std::vector<T>& B, std::size_t s) {
    std::size_t n = r - p + 1;
    if (n == 1) {
        B[s] = A[p];
    } else {
        std::vector<T> C (n);
        std::size_t q = (p + r) / 2;
        std::size_t q2 = q - p + 1;
        {
            std::jthread th(&PMedianMergeSort<T>, std::ref(A), p, q, std::ref(C), 0);
            PMedianMergeSort(A, q + 1, r, C, q2);
        }
        PMedianMerge(C, 0, q2 - 1, q2, n - 1, B, s);
    }
}

int main() {
    constexpr std::size_t N = 200;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    std::mt19937 gen(std::random_device{}());
    sr::shuffle (v, gen);
    auto v2 = v;
    auto v3 = v;
    std::vector<int> B (N);
    auto t1 = crn::steady_clock::now();
    PMergeSort(v, 0, N - 1, B, 0);
    auto t2 = crn::steady_clock::now();
    auto dt1 = crn::duration_cast<crn::microseconds>(t2 - t1);
    assert(sr::is_sorted(B));
    std::cout << "Aggressively parallelized merge sort : " << dt1.count() << "us\n";
    auto t3 = crn::steady_clock::now();
    sr::sort(v2);
    auto t4 = crn::steady_clock::now();
    auto dt2 = crn::duration_cast<crn::microseconds>(t4 - t3);
    std::cout << "std::sort : " << dt2.count() << "us\n";
    std::vector<int> C (N);
    auto t5 = crn::steady_clock::now();
    PMedianMergeSort(v3, 0, N - 1, C, 0);
    auto t6 = crn::steady_clock::now();
    auto dt3 = crn::duration_cast<crn::microseconds>(t6 - t5);
    assert(sr::is_sorted(C));
    std::cout << "Aggressively parallelized median merge sort : " << dt3.count() << "us\n";
}
