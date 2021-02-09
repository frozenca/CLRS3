#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>

std::vector<size_t> get_argmax(const std::vector<std::vector<int>>& monge) {
    std::vector<size_t> res (monge.size());
    if (monge.size() == 1) {
        res[0] = std::distance(monge[0].begin(), std::min_element(monge[0].begin(), monge[0].end()));
        return res;
    } else if (monge.size() < 4) {
        res[1] = std::distance(monge[1].begin(), std::min_element(monge[1].begin(), monge[1].end()));
        res[0] = std::distance(monge[0].begin(), std::min_element(monge[0].begin(), monge[0].begin() + res[1] + 1));
        if (monge.size() == 3) {
            res[2] = std::distance(monge[2].begin(), std::min_element(monge[2].begin() + res[1], monge[2].end()));
        }
        return res;
    }
    std::vector<std::vector<int>> monge_evens;
    for (size_t r = 1; r < monge.size(); r += 2) {
        monge_evens.push_back(monge[r]);
    }
    std::vector<size_t> argmax_evens = get_argmax(monge_evens);
    for (size_t r = 1, re = 0; r < monge.size(); r += 2, re++) {
        res[r] = argmax_evens[re];
    }
    if (monge.size() % 2 == 1) {
        res.push_back(monge[0].size() - 1);
    }
    res[0] = std::distance(monge[0].begin(), std::min_element(monge[0].begin(), monge[0].begin() + res[1] + 1));
    for (size_t r = 2; r < monge.size(); r += 2) {
        res[r] = std::distance(monge[r].begin(), std::min_element(monge[r].begin() + res[r - 1],
                monge[r].begin() + res[r + 1] + 1));
    }
    if (monge.size() % 2 == 1) {
        res.pop_back();
    }
    return res;
}

int main() {
    std::vector<std::vector<int>> monge {{10, 17, 13, 28, 23},
                                         {17, 22, 16, 29, 23},
                                         {24, 28, 22, 34, 24},
                                         {11, 13, 6, 17, 7},
                                         {45, 44, 32, 37, 23},
                                         {36, 33, 19, 21, 6},
                                         {75, 66, 51, 53, 34}};

    auto argmaxes = get_argmax(monge);
    for (auto n : argmaxes) {
        std::cout << n << '\n';
    }

}