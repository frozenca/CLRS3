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

bool isPolygon(const std::vector<Point2d>& P) {
    if (P.size() < 3) {
        return false;
    }
    int is_left_turn = 0;
    for (std::size_t i = 0; i < P.size(); i++) {
        const auto& p0 = P[i % P.size()];
        const auto& p1 = P[(i + 1) % P.size()];
        const auto& p2 = P[(i + 2) % P.size()];
        auto cross = Cross(p1 - p0, p2 - p0);
        if (isClose(cross, 0.0)) {
            return false;
        } else if (cross > 0.0) {
            if (is_left_turn == -1) {
                return false;
            }
            is_left_turn = 1;
        } else {
            if (is_left_turn == 1) {
                return false;
            }
            is_left_turn = -1;
        }
    }
    return true;
}

int main() {
    std::vector<Point2d> P1 {{0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0},
                            {2.0, 2.0}, {0.0, 2.0}};

    assert(!isPolygon(P1));

    std::vector<Point2d> P2 {{0.0, 0.0}, {2.0, 0.0},
                            {2.0, 2.0}, {0.0, 2.0}};

    assert(isPolygon(P2));

}