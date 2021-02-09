#include <algorithm>
#include <cassert>
#include <vector>
#include <utility>
#include <ranges>

namespace sr = std::ranges;

std::vector<std::pair<double, double>> unitIntervals(std::vector<double>& points) {
    sr::sort(points);
    std::vector<std::pair<double, double>> intvs;
    auto start = points[0];
    intvs.emplace_back(start, start + 1.0);
    for (auto point : points) {
        if (point <= start + 1.0) {
            continue;
        } else {
            start = point;
            intvs.emplace_back(start, start + 1.0);
        }
    }
    return intvs;
}

int main() {
}
