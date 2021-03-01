#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
#include <vector>
#include <ranges>

bool isClose(double x, double d) {
    return std::fabs(x - d) < 1e-6;
}

namespace sr = std::ranges;

struct Point2d {
    double x = 0.0;
    double y = 0.0;
    Point2d() = default;
    Point2d(double x, double y) : x {x}, y {y} {}
};

Point2d operator+(const Point2d& lhs, const Point2d& rhs) {
    return Point2d(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point2d operator-(const Point2d& lhs, const Point2d& rhs) {
    return Point2d(lhs.x - rhs.x, lhs.y - rhs.y);
}

bool operator==(const Point2d& lhs, const Point2d& rhs) {
    return isClose(lhs.x, rhs.x) && isClose(lhs.y, rhs.y);
}

bool operator!=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs == rhs);
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double dist(const Point2d& lhs, const Point2d& rhs) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

double Area(const std::vector<Point2d>& P) {
    double area = 0.0;
    for (std::size_t i = 0; i < P.size(); i++) {
        const auto& p1 = P[i % P.size()];
        const auto& p2 = P[(i + 1) % P.size()];
        area += Cross(p1, p2);
    }
    return area / 2.0;
}

int main() {
    std::vector<Point2d> P {{0.0, 0.0},
                            {2.0, 0.0},
                            {1.0, 2.0}};
    std::cout << Area(P) << '\n';

    std::vector<Point2d> P2 {{0.0, 0.0},
                            {1.0, 1.0},
                            {2.0, 0.0},
                             {2.0, 3.0},
                             {1.0, 2.0},
                             {0.0, 3.0}};
    std::cout << Area(P2) << '\n';


}