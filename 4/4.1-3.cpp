#include <chrono>
#include <iostream>
#include <limits>
#include <tuple>
#include <random>
#include <vector>

using ans_type = std::tuple<size_t, size_t, int>;

ans_type solveCrossing(std::vector<int>& A, size_t low, size_t mid, size_t high) {
    int l_sum = std::numeric_limits<int>::min();
    int sum = 0;
    size_t max_left = 0;
    for (size_t i = mid; i >= low && i < high; --i) {
        sum += A[i];
        if (sum > l_sum) {
            l_sum = sum;
            max_left = i;
        }
    }
    int r_sum = std::numeric_limits<int>::min();
    sum = 0;
    size_t max_right = 0;
    for (size_t i = mid + 1; i <= high; ++i) {
        sum += A[i];
        if (sum > r_sum) {
            r_sum = sum;
            max_right = i;
        }
    }
    return {max_left, max_right, l_sum + r_sum};
}

ans_type solveRecursive(std::vector<int>& A, size_t low, size_t high) {
    if (high == low) {
        return {low, high, A[low]};
    } else {
        size_t mid = low + (high - low) / 2;
        auto [l_low, l_high, l_sum] = solveRecursive(A, low, mid);
        auto [r_low, r_high, r_sum] = solveRecursive(A, mid + 1, high);
        auto [c_low, c_high, c_sum] = solveCrossing(A, low, mid, high);
        if (l_sum >= r_sum && l_sum >= c_sum) {
            return {l_low, l_high, l_sum};
        } else if (r_sum >= l_sum && r_sum >= c_sum) {
            return {r_low, r_high, r_sum};
        } else {
            return {c_low, c_high, c_sum};
        }
    }
}

ans_type solveBruteForce(std::vector<int>& A) {
    int max_sum = std::numeric_limits<int>::min();
    size_t max_left = 0, max_right = 0;
    for (size_t i = 0; i < A.size(); i++) {
        int sum = 0;
        for (size_t j = i; j < A.size(); j++) {
            sum += A[j];
            if (max_sum < sum) {
                max_left = i;
                max_right = j;
                max_sum = sum;
            }
        }
    }
    return {max_left, max_right, max_sum};
}

int main() {
    constexpr size_t TRIALS = 10000;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> generator (-100, 100);

    size_t LENGTH = 10;
    namespace crn = std::chrono;

    while (true) {
        std::cout << "Comparing two algorithms with " << LENGTH << " sized input\n";
        crn::microseconds diff(0);
        for (size_t i = 0; i < TRIALS; i++) {
            std::vector<int> v(LENGTH);
            for (size_t j = 0; j < LENGTH; j++) {
                v[j] = generator(gen);
            }
            auto start = crn::steady_clock::now();
            auto[l, h, s] = solveRecursive(v, 0, v.size() - 1);
            auto end = crn::steady_clock::now();
            diff += crn::duration_cast<crn::microseconds>(end - start);
        }
        diff /= TRIALS;
        std::cout << "Recursive: " << diff.count() << " us on average.\n";
        crn::microseconds diff2(0);
        for (size_t i = 0; i < TRIALS; i++) {
            std::vector<int> v(LENGTH);
            for (size_t j = 0; j < LENGTH; j++) {
                v[j] = generator(gen);
            }
            auto start = crn::steady_clock::now();
            auto[l, h, s] = solveBruteForce(v);
            auto end = crn::steady_clock::now();
            diff2 += crn::duration_cast<crn::microseconds>(end - start);
        }
        diff2 /= TRIALS;
        std::cout << "Bruteforce: " << diff2.count() << " us on average.\n";
        if (diff2 > diff) {
            std::cout << "Recursive finally beats bruteforce, terminating\n";
            break;
        } else {
            LENGTH += 10;
        }
    }




    return 0;
}