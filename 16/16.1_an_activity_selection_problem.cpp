#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <ranges>

namespace sr = std::ranges;

std::vector<std::pair<int, int>> RecursiveActivitySelectorHelper(std::vector<std::pair<int, int>>& intervals, size_t k) {
    size_t m = k + 1;
    while (m < intervals.size() && intervals[m].first < intervals[k].second) {
        m++;
    }
    if (m < intervals.size()) {
        auto A = RecursiveActivitySelectorHelper(intervals, m);
        A.push_back(intervals[m]);
        return A;
    } else {
        return std::vector<std::pair<int, int>>();
    }
}

std::vector<std::pair<int, int>> RecursiveActivitySelector(std::vector<std::pair<int, int>>& intervals) {
    sr::sort(intervals, [](auto& a, auto& b) {return a.second < b.second;});
    auto A = RecursiveActivitySelectorHelper(intervals, 0);
    return A;
}

std::vector<std::pair<int, int>> GreedyActivitySelector(std::vector<std::pair<int, int>>& intervals) {
    sr::sort(intervals, [](auto& a, auto& b) {return a.second < b.second;});
    size_t n = intervals.size();
    std::vector<std::pair<int, int>> A;
    A.push_back(intervals[0]);
    size_t k = 0;
    for (size_t m = 1; m < n; m++) {
        if (intervals[m].first >= intervals[k].second) {
            A.push_back(intervals[m]);
            k = m;
        }
    }
    return A;
}

int main() {
}
