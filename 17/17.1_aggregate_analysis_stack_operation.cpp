#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <stack>

namespace crn = std::chrono;

void Pop(std::stack<int>& s) {
    if (!s.empty()) {
        s.pop();
    }
}

void Push(std::stack<int>& s) {
    s.push(1);
}

void Multipop(std::stack<int>& s, std::size_t k) {
    while (!s.empty() && k) {
        s.pop();
        k--;
    }
}

int main() {
    constexpr std::size_t SIZE = 1'000'000;

    std::stack<int> s;
    auto t1 = crn::steady_clock::now();
    for (std::size_t i = 0; i < SIZE / 2; i++) {
        Push(s);
    }
    for (std::size_t i = 0; i < SIZE / 2; i++) {
        Pop(s);
    }
    auto t2 = crn::steady_clock::now();
    auto d1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(1, 3);
    t1 = crn::steady_clock::now();
    for (std::size_t i = 0; i < SIZE; i++) {
        switch (dist(gen)) {
            case 1:
                Push(s);
                break;
            case 2:
                Pop(s);
                break;
            case 3:
                Multipop(s, 20);
                break;
        }
    }
    t2 = crn::steady_clock::now();
    d1 = crn::duration_cast<crn::milliseconds>(t2 - t1);
    std::cout << d1.count() << "ms\n";

}