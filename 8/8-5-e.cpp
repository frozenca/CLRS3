#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <queue>
#include <utility>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

std::mt19937 gen(std::random_device{}());

using it_p_t = std::pair<std::vector<int>::const_iterator,
        std::vector<int>::const_iterator>;

std::vector<int> mergeKLists(const std::vector<std::vector<int>>& lists) {
    auto nodeComp = [](it_p_t& na, it_p_t& nb) {
        return *(na.first) > *(nb.first);
    };
    std::priority_queue<it_p_t, std::vector<it_p_t>, decltype(nodeComp)> iters(nodeComp);
    std::vector<int> merged_vec;
    if (lists.empty()) return merged_vec;
    for (const auto& list : lists) {
        if (!list.empty()) {
            iters.emplace(list.cbegin(), list.cend());
        }
    }
    if (iters.empty()) return merged_vec;
    auto curr_iter = iters.top();
    merged_vec.push_back(*(curr_iter.first));
    while (curr_iter.first != curr_iter.second) {
        iters.pop();
        if (++curr_iter.first != curr_iter.second) {
            iters.push(curr_iter);
        }
        if (iters.empty()) {
            break;
        }
        curr_iter = iters.top();
        merged_vec.push_back(*(iters.top().first));
    }
    return merged_vec;
}

void averageSort(std::vector<int>& v, size_t k) {
    std::vector<std::vector<int>> V (k);
    for (size_t i = 0; i < v.size(); i++) {
        V[i % k].push_back(v[i]);
    }
    for (auto& V_ : V) {
        sr::sort(V_);
    }
    v = mergeKLists(V);
}

int main() {
    constexpr size_t N = 500;

    std::vector<int> v (N);
    std::iota(v.begin(), v.end(), 0);
    sr::shuffle(v, gen);
    size_t k = 10;
    averageSort(v, k);
    assert(sr::is_sorted(v));
}