#include <random>
#include <iostream>
#include <vector>
#include <utility>
#include <map>

std::vector<std::vector<double>> FirstFit(const std::vector<double>& S) {
    std::map<double, std::size_t> B;
    std::vector<std::vector<double>> L;
    for (auto s : S) {
        auto it = B.lower_bound(s);
        if (it == B.end()) {
            std::vector<double> L_b {s};
            L.push_back(std::move(L_b));
            B[1 - s] = L.size() - 1;
        } else {
            auto [e, j] = *it;
            L[j].push_back(s);
            B.erase(e);
            if (e > s) {
                B[e - s] = j;
            }
        }
    }
    return L;
}

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(0.0001, 0.9999);
    constexpr std::size_t n = 20;
    std::vector<double> S;
    S.reserve(n);
    for (std::size_t i = 0; i < n; i++) {
        S.push_back(dist(gen));
    }

    auto FFit = FirstFit(S);
    for (const auto& bin : FFit) {
        for (auto num : bin) {
            std::cout << num << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

}