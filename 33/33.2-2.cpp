#include <algorithm>
#include <cassert>
#include <cmath>
#include <compare>
#include <iostream>
#include <vector>
#include <utility>
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

struct Segment {
    bool directed = false;
    Point2d src;
    Point2d dst;
    Segment(const Point2d& src, const Point2d& dst, bool directed = false)
            : src{src}, dst{dst}, directed {directed} {
        assert(src != dst);
    }
};

auto Above(const Segment& s1, const Segment& s2, double x) {
    Point2d v1 = s1.dst - s1.src;
    Point2d v2 = s2.dst - s2.src;

    if (v1.x * v2.x >= 0) {
        return (v1.y * (x - s1.src.x) + v1.x * s1.src.y) * v2.x <=> (v2.y * (x - s2.src.x) + v2.x * s2.src.y) * v1.x;
    } else {
        return -1.0 * (v1.y * (x - s1.src.x) + v1.x * s1.src.y) * v2.x <=> -1.0 * (v2.y * (x - s2.src.x) + v2.x * s2.src.y) * v1.x;
    }
}

int main() {
    Point2d p1 {1.0, 1.0};
    Point2d p2 {0.0, 2.0};
    Point2d p3 {3.0, 0.0};
    Point2d p4 {4.0, 1.0};
    Point2d p5 {4.0, 0.0};
    Point2d p6 {4.0, -1.0};

    Segment s1 {p1, p2};
    Segment s2 {p3, p4};
    Segment s3 {p3, p5};
    Segment s4 {p3, p6};

    assert(Above(s1, s2, 2.0) == std::partial_ordering::less);
    assert(Above(s1, s3, 2.0) == std::partial_ordering::equivalent);
    assert(Above(s1, s4, 2.0) == std::partial_ordering::greater);
}