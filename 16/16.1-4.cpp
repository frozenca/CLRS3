#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <set>
#include <tuple>
#include <map>
#include <ranges>

namespace sr = std::ranges;

std::map<int, size_t> GreedyActivitySelector(std::vector<std::pair<int, int>>& intervals) {
    std::vector<std::tuple<int, int, bool>> endpoints;
    for (const auto& [start, finish] : intervals) {
        endpoints.emplace_back(start, start, true);
        endpoints.emplace_back(finish, start, false);
    }
    sr::sort(endpoints);
    size_t n = intervals.size();
    std::set<size_t> available;
    std::map<int, size_t> used_map;
    for (size_t i = 0; i < n; i++) {
        available.insert(i);
    }
    for (const auto& [point, startOf, isStart] : endpoints) {
        if (isStart) {
            auto least_available = *available.begin();
            available.erase(least_available);
            used_map[point] = least_available;
        } else {
            auto being_freed = used_map[startOf];
            available.insert(being_freed);
        }
    }

    return used_map;
}

int main() {
}
