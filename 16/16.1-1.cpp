#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <ranges>

namespace sr = std::ranges;

std::pair<std::vector<std::vector<size_t>>, std::vector<std::vector<size_t>>>
        DynamicActivitySelect(std::vector<std::pair<int, int>>& intervals) {
    sr::sort(intervals, [](auto& a, auto& b) {return a.second < b.second;});
    size_t n = intervals.size();
    std::vector<std::vector<size_t>> c(n + 2, std::vector<size_t>(n + 2));
    std::vector<std::vector<size_t>> a(n + 2, std::vector<size_t>(n + 2));
    for (size_t l = 2; l <= n + 1; l++) {
        for (size_t i = 0; i <= n - l  + 1; i++) {
            size_t j = i + l;
            size_t k = j - 1;
            while (intervals[i].second < intervals[k].second) {
                if (intervals[i].second <= intervals[k].first && intervals[k].second <= intervals[j].first
                && c[i][k] + c[k][j] + 1 > c[i][j]) {
                    c[i][j] = c[i][k] + c[k][j] + 1;
                    a[i][j] = k;
                }
                k--;
            }
        }
    }
    return {c, a};
}


int main() {
}
