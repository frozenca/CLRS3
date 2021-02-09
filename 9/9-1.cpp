#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>
#include <queue>
#include <vector>
#include <ranges>

namespace sr = std::ranges;
namespace srv = std::ranges::views;

template <typename T>
std::vector<T> largestIthSorted1(const std::vector<T>& A, size_t i) {
    auto B = A;
    sr::sort(B);
    std::vector<T> C (std::make_move_iterator(B.begin() + B.size() - i), std::make_move_iterator(B.end()));
    return C;
}

template <typename T>
std::vector<T> largestIthSorted2(const std::vector<T>& A, size_t i) {
    std::priority_queue<T> q (A.begin(), A.end());
    std::vector<T> C;
    for (size_t j = 0; j < i; j++) {
        C.push_back(q.top());
        q.pop();
    }
    sr::reverse(C);
    return C;
}

template <typename T>
std::vector<T> largestIthSorted3(const std::vector<T>& A, size_t i) {
    auto B = A;
    sr::nth_element(B, B.begin() + B.size() - i);
    sr::sort(B | srv::drop(B.size() - i));
    std::vector<T> C (std::make_move_iterator(B.begin() + B.size() - i), std::make_move_iterator(B.end()));
    return C;
}

int main() {
    std::vector<int> v {3, 1, 6, 2, 5, 4, 7, 9, 8};
    size_t i = 4;
    auto a1 = largestIthSorted1(v, i);
    for (auto n : a1) {
        std::cout << n << ' ';
    }
    std::cout << '\n';
    auto a2 = largestIthSorted2(v, i);
    for (auto n : a2) {
        std::cout << n << ' ';
    }
    std::cout << '\n';
    auto a3 = largestIthSorted3(v, i);
    for (auto n : a3) {
        std::cout << n << ' ';
    }
    std::cout << '\n';


}