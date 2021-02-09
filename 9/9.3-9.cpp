#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

double findOptimalPlace(std::vector<std::pair<double, double>>& A) {
    auto compY = [](auto& p1, auto& p2) {return p1.second < p2.second;};
    sr::nth_element(A, A.begin() + A.size() / 2, compY);
    return A[A.size() / 2].second;
}

int main() {
    std::vector<std::pair<double, double>> A {{1, 3}, {2, 5}, {3, 1}, {4, 2}, {5, 4}};
    std::cout << findOptimalPlace(A);

}