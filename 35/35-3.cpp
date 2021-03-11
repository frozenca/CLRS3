#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
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

std::vector<std::unordered_set<std::size_t>> GreedyWeightedSetCover(const std::unordered_set<std::size_t>& X,
                                                            const std::vector<std::unordered_set<std::size_t>>& F,
                                                            const std::vector<double>& W) {
    assert(F.size() == W.size());
    auto U = X;
    std::vector<std::unordered_set<std::size_t>> C;
    while (!U.empty()) {
        std::size_t S_idx = -1;
        double inter_size = 0;
        for (std::size_t i = 0; i < F.size(); i++) {
            double curr_inter_size = getIntersectionSize(U, F[i]) / W[i];
            if (inter_size < curr_inter_size) {
                inter_size = curr_inter_size;
                S_idx = i;
            }
        }
        if (S_idx != -1) {
            const auto& S = F[S_idx];
            for (auto s : S) {
                U.erase(s);
            }
            C.push_back(S);
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
    std::vector<double> W {
        1, 2, 3, 4, 5, 6
    };
    auto covers = GreedyWeightedSetCover(X, F, W);
    for (const auto& cover : covers) {
        for (auto elt : cover) {
            std::cout << elt << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

}