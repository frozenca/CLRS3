#include <algorithm>
#include <cassert>
#include <vector>
#include <tuple>
#include <ranges>

namespace sr = std::ranges;

std::pair<std::vector<std::vector<size_t>>, std::vector<std::vector<size_t>>>
DynamicActivitySelect(std::vector<std::tuple<int, int, int>>& intervals) {
    sr::sort(intervals, [](auto& a, auto& b) {return std::get<1>(a) < std::get<1>(b);});
    size_t n = intervals.size();
    std::vector<std::vector<size_t>> c(n + 2, std::vector<size_t>(n + 2));
    std::vector<std::vector<size_t>> a(n + 2, std::vector<size_t>(n + 2));
    for (size_t l = 2; l <= n + 1; l++) {
        for (size_t i = 0; i <= n - l  + 1; i++) {
            size_t j = i + l;
            size_t k = j - 1;
            while (std::get<1>(intervals[i]) < std::get<1>(intervals[k])) {
                if (std::get<1>(intervals[i]) <= std::get<0>(intervals[k]) && std::get<1>(intervals[k]) <= std::get<0>(intervals[j])
                    && c[i][k] + c[k][j] + std::get<2>(intervals[k]) > c[i][j]) {
                    c[i][j] = c[i][k] + c[k][j] + std::get<2>(intervals[k]);
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
