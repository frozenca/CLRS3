#include <cassert>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>

int maxSubArray(const std::vector<int>& nums) {
    if (nums.empty()) return 0;
    int runningSum = 0, maxSum = nums[0];
    for (auto n : nums) {
        runningSum += n;
        maxSum = std::max(runningSum, maxSum);
        runningSum = std::max(0, runningSum);
    }
    return maxSum;
}

int main() {
    assert(maxSubArray(std::vector<int>({-1})) == -1);
    assert(maxSubArray(std::vector<int>({-2, 1, -3, 4, -1, 2, 1, -5, 4})) == 6);
    assert(maxSubArray(std::vector<int>({})) == 0);

    constexpr int SIZE = 10'000'000;
    std::vector<int> v (SIZE * 2);
    std::iota(v.begin(), v.end(), -SIZE);
    std::shuffle(v.begin(), v.end(), std::mt19937(std::random_device{}()));

    namespace crn = std::chrono;
    auto begin = crn::steady_clock::now();
    auto n = maxSubArray(v);
    auto end = crn::steady_clock::now();
    auto diff = crn::duration_cast<crn::milliseconds>(end - begin);
    assert(diff.count() < 1000);
    std::cout << "OK";

    return EXIT_SUCCESS;
}