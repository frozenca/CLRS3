#include <algorithm>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

namespace sr = std::ranges;

std::size_t ExactSubsetSum(const std::vector<std::size_t>& S, std::size_t t) {
    const auto n = S.size();
    std::vector<std::size_t> L {0};
    for (auto s : S) {
        auto L_plus = L;
        sr::for_each(L_plus, [&s](auto& e){e += s;});
        std::vector<std::size_t> L_next;
        sr::merge(L, L_plus, std::back_inserter(L_next));
        std::swap(L, L_next);
        L.erase(sr::upper_bound(L, t), L.end());
    }
    return L.back();
}

std::vector<std::size_t> Trim(const std::vector<std::size_t>& L, double delta) {
    if (L.empty()) {
        return {};
    }
    const auto m = L.size();
    std::vector<std::size_t> L_prime;
    L_prime.push_back(L.back());
    auto last = L.back();
    for (std::size_t i = m - 2; i < m; i--) {
        if (L[i] > static_cast<int>(last * (1.0 + delta))) {
            L_prime.push_back(L[i]);
            last = L[i];
        }
    }
    return L_prime;
}

std::size_t ApproxSubsetSumGreater(const std::vector<std::size_t>& S, std::size_t t, double eps) {
    const auto n = S.size();
    std::size_t sum = std::accumulate(S.begin(), S.end(), 0ul);
    std::vector<std::size_t> L {sum};
    for (auto s : S) {
        auto L_plus = L;
        sr::for_each(L_plus, [&s](auto& e){e -= s;});
        std::vector<std::size_t> L_next;
        sr::merge(L, L_plus, std::back_inserter(L_next));
        L_next = Trim(L_next, eps / (2 * n));
        std::swap(L, L_next);
        L.erase(L.begin(),sr::lower_bound(L, t));
    }
    return L.back();
}

int main() {
    std::vector<std::size_t> S {104, 102, 201, 101};
    std::size_t t = 308;
    double eps = 0.40;
    auto exact_ss = ExactSubsetSum(S, t);
    std::cout << exact_ss << '\n';
    auto approx_ss = ApproxSubsetSumGreater(S, t, eps);
    std::cout << approx_ss << '\n';
}