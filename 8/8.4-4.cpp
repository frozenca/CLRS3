#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>
#include <iostream>

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

void bucketSort(std::vector<std::pair<double, double>>& A) {
    std::vector<std::vector<std::pair<double, double>>> B (A.size());
    for (const auto& [x, y] : A) {
        auto r = std::hypot(x, y);
        B[std::floor(std::pow(r, 2.0) * A.size())].emplace_back(x, y);
    }
    for (auto& v : B) {
        insertionSort(v,
                      [](auto &a, auto &b) { return std::hypot(a.first, a.second) > std::hypot(b.first, b.second); });
    }
    A.clear();
    for (auto& v : B) {
        A.insert(A.end(), std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
    }
}

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> radius(0.0, 1.0);
    std::uniform_real_distribution<> angle(0.0, 2 * M_PI);
    constexpr size_t N = 100;
    std::vector<std::pair<double, double>> A;

    for (size_t i = 0; i < N; i++) {
        auto r = radius(gen);
        auto t = angle(gen);
        A.emplace_back(r * std::cos(t), r * std::sin(t));
    }

    bucketSort(A);

    for (const auto& [x, y] : A) {
        std::cout << "(" << x << ", " << y << ") : r = " << std::hypot(x, y) << '\n';
    }



}