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

struct Segment {
    bool directed = false;
    Point2d src;
    Point2d dst;
    Segment(const Point2d& src, const Point2d& dst, bool directed = false)
            : src{src}, dst{dst}, directed {directed} {
        assert(src != dst);
    }
};

double Direction(const Point2d& p1, const Point2d& p2, const Point2d& p3) {
    return Cross(p3 - p1, p2 - p1);
}

bool OnSegment(const Point2d& p1, const Point2d& p2, const Point2d& p3) {
    if ((std::min(p1.x, p2.x) <= p3.x && p3.x <= std::max(p1.x, p2.x))
        && (std::min(p1.y, p2.y) <= p3.y && p3.y <= std::max(p1.y, p2.y))) {
        return true;
    } else {
        return false;
    }
}

bool SegmentsIntersect(const Segment& s1, const Segment& s2) {
    const auto& p1 = s1.src;
    const auto& p2 = s1.dst;
    const auto& p3 = s2.src;
    const auto& p4 = s2.dst;
    auto d1 = Direction(p3, p4, p1);
    auto d2 = Direction(p3, p4, p2);
    auto d3 = Direction(p1, p2, p3);
    auto d4 = Direction(p1, p2, p4);
    if (((d1 > 0.0 && d2 < 0.0) || (d1 < 0.0 && d2 > 0.0))
        && ((d3 > 0.0 && d4 < 0.0) || (d3 < 0.0 && d4 > 0.0))) {
        return true;
    } else if (isClose(d1, 0.0) && OnSegment(p3, p4, p1)) {
        return true;
    } else if (isClose(d2, 0.0) && OnSegment(p3, p4, p2)) {
        return true;
    } else if (isClose(d3, 0.0) && OnSegment(p1, p2, p3)) {
        return true;
    } else if (isClose(d4, 0.0) && OnSegment(p1, p2, p4)) {
        return true;
    }
    return false;
}

bool isRightHorizontalIntersect(const Point2d& p0, const Point2d& p1, const Point2d& p2) {
    Point2d p3 {std::max(p1.x, p2.x), p0.y};
    if (p0.x > p3.x) {
        return false;
    } else if (isClose(p0.x, p3.x)) {
        return isClose(p1.x, p2.x) && OnSegment(p1, p2, p0);
    }
    Segment s1 {p0, p3};
    Segment s2 {p1, p2};
    return SegmentsIntersect(s1, s2);
}

int main() {
    Point2d p0 {0.0, 0.0};
    Point2d p1 {1.0, 2.0};
    Point2d p2 {3.0, 5.0};
    Point2d p3 {3.0, -5.0};
    Point2d p4 {-3.0, 5.0};
    Point2d p5 {-3.0, -5.0};
    assert(!isRightHorizontalIntersect(p0, p1, p2));
    assert(isRightHorizontalIntersect(p0, p1, p3));
    assert(!isRightHorizontalIntersect(p0, p4, p5));

}