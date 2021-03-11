#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <unordered_set>
#include <vector>

std::size_t getIntersectionSize(const std::unordered_set<std::size_t>& S1,
                                const std::unordered_set<std::size_t>& S2) {
    std::size_t n = 0;
    if (S1.size() < S2.size()) {
        for (auto s : S1) {
            if (S2.contains(s)) {
                n++;
            }
        }
    } else {
        for (auto s : S2) {
            if (S1.contains(s)) {
                n++;
            }
        }
    }
    return n;
}

std::unordered_set<std::size_t> LinearGreedySetCover(const std::unordered_set<std::size_t>& X,
                                                            const std::vector<std::unordered_set<std::size_t>>& F_) {
    auto F = F_;
    if (F.empty()) {
        return {};
    }
    std::size_t max_s = 0;
    for (const auto& S : F) {
        max_s = std::max(max_s, S.size());
    }
    std::vector<std::unordered_set<std::size_t>> A (max_s + 1);
    for (std::size_t i = 0; i < F.size(); i++) {
        A[F[i].size()].insert(i);
    }
    std::size_t A_max = max_s;
    std::unordered_map<std::size_t, std::unordered_set<std::size_t>> L;
    for (std::size_t i = 0; i < F.size(); i++) {
        for (auto l : F[i]) {
            L[l].insert(i);
        }
    }

    std::unordered_set<std::size_t> C;
    std::unordered_set<std::size_t> T;
    while (A_max > 0) {
        auto S_0_idx = *A[A_max].begin();
        C.insert(S_0_idx);
        A[A_max].erase(S_0_idx);
        for (auto l : F[S_0_idx]) {
            if (!T.contains(l)) {
                for (auto S_idx : L[l]) {
                    A[F[S_idx].size()].erase(S_idx);
                    F[S_idx].erase(l);
                    A[F[S_idx].size()].insert(S_idx);
                    if (A[A_max].empty() && A_max > 0) {
                        A_max--;
                    }
                }
                T.insert(l);
            }
        }
    }
    return C;

}

int main() {
    std::unordered_set<std::size_t> X {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::vector<std::unordered_set<std::size_t>> F {
            {0, 1, 2, 3, 4, 5},
            {4, 5, 7, 8},
            {0, 3, 6, 9},
            {1, 4, 6, 7, 10},
            {2, 5, 8, 11},
            {9, 10}
    };
    auto covers = LinearGreedySetCover(X, F);
    for (const auto& cover : covers) {
        for (auto elt : F[cover]) {
            std::cout << elt << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

}