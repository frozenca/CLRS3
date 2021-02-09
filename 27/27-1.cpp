#include <algorithm>
#include <cassert>
#include <cmath>
#include <execution>
#include <functional>
#include <random>
#include <iostream>
#include <vector>
#include <ranges>
#include <thread>

namespace se = std::execution;
namespace sr = std::ranges;

template <typename T>
void SumArrays(const std::vector<T>& A, const std::vector<T>& B, std::vector<T>& C) {
    std::transform(se::unseq, A.begin(), A.end(), B.begin(), C.begin(), std::plus<>{});
}

int main() {
    constexpr std::size_t N = 20;

    std::mt19937 gen(std::random_device{}());
    std::vector<int> A (N);
    std::iota(A.begin(), A.end(), 0);
    std::vector<int> B (N);
    std::iota(B.begin(), B.end(), 0);
    sr::shuffle(A, gen);
    sr::shuffle(B, gen);
    std::vector<int> C (N);
    SumArrays(A, B, C);
    for (auto n : C) {
        std::cout << n << ' ';
    }
    std::cout << '\n';


}