#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <ranges>

namespace sr = std::ranges;

std::vector<std::pair<int, int>> GreedyActivitySelector(std::vector<std::pair<int, int>>& intervals) {
    sr::sort(intervals, [](auto& a, auto& b) {return a.second < b.second;});
    size_t n = intervals.size();
    std::vector<std::pair<int, int>> A;
    A.push_back(intervals.back());
    size_t k = n - 1;
    for (size_t m = n - 2; m < n; m--) {
        if (intervals[m].second <= intervals[k].first) {
            A.push_back(intervals[m]);
            k = m;
        }
    }
    return A;
}

int main() {
}
