#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>
#include <vector>
#include <utility>

using Point = std::pair<double, double>;

double distance(const Point& p1, const Point& p2) {
    return std::hypot(p1.first - p2.first, p1.second - p2.second);
}

std::pair<std::vector<std::vector<double>>, std::vector<std::vector<size_t>>> EuclideanTSP(std::vector<Point>& p) {
    std::sort(p.begin(), p.end());
    size_t n = p.size();
    std::vector<std::vector<double>> b (n, std::vector<double> (n));
    std::vector<std::vector<size_t>> r (n, std::vector<size_t> (n));
    b[0][1] = distance(p[0], p[1]);
    for (size_t j = 2; j < n; j++) {
        for (size_t i = 0; i < j - 1; i++) {
            b[i][j] = b[i][j - 1] + distance(p[j - 1], p[j]);
            r[i][j] = j - 1;
        }
        b[j - 1][j] = std::numeric_limits<double>::max();
        for (size_t k = 0; k < j - 1; k++) {
            auto q = b[k][j - 1] + distance(p[k], p[j]);
            if (q < b[j - 1][j]) {
                b[j - 1][j] = q;
                r[j - 1][j] = k;
            }
        }
    }
    b[n - 1][n - 1] = b[n - 2][n - 1] + distance(p[n - 2], p[n - 1]);
    return {b, r};
}

int main() {
    std::vector<Point> p;
    p.emplace_back(0.0, 0.0);
    p.emplace_back(1.0, 6.0);
    p.emplace_back(2.0, 3.0);
    p.emplace_back(5.0, 2.0);
    p.emplace_back(6.0, 5.0);
    p.emplace_back(7.0, 1.0);
    p.emplace_back(8.0, 4.0);
    auto [b, r] = EuclideanTSP(p);
    std::cout << b.back().back();

}