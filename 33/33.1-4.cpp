#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>
#include <vector>
#include <ranges>

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
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs == rhs);
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

bool isClose(double x, double d) {
    return std::fabs(x - d) < 1e-6;
}

double dist(const Point2d& lhs, const Point2d& rhs) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

bool hasColinear(const std::vector<Point2d>& P) {
    for (const auto& p : P) {
        auto sort_by_angle_with_p = [&p](const auto& p1, const auto& p2) {
            double cross = Cross(p1 - p, p2 - p);
            return cross > 0.0;
        };
        std::set<Point2d, decltype(sort_by_angle_with_p)> S(sort_by_angle_with_p);
        for (const auto& q : P) {
            if (S.contains(q)) {
                return true;
            }
            S.insert(q);
        }
    }
    return false;
}

int main() {
    std::vector<Point2d> P {{0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0},
                            {0.0, 1.0}, {1.0, 1.0}, {2.0, 1.0},
                            {0.0, 2.0}, {1.0, 2.0}, {2.0, 2.0}};

    assert(hasColinear(P));
}