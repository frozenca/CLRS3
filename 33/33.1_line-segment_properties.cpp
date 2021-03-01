#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>

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

struct Segment {
    bool directed = false;
    Point2d src;
    Point2d dst;
    Segment(const Point2d& src, const Point2d& dst, bool directed = false)
        : src{src}, dst{dst}, directed {directed} {
        assert(src != dst);
    }
};

enum class Orientation {
    Clockwise,
    Counterclockwise,
    Colinear
};

bool isClose(double x, double d) {
    return std::fabs(x - d) < 1e-6;
}

Orientation getOrientation(const Segment& s1, const Segment& s2) {
    assert(s1.src == s2.src);
    Point2d s1vec = s1.dst - s1.src;
    Point2d s2vec = s2.dst - s2.src;
    double cross = Cross(s1vec, s2vec);
    if (cross > 0.0) {
        return Orientation::Clockwise;
    } else if (isClose(cross, 0.0)) {
        return Orientation::Colinear;
    } else {
        return Orientation::Counterclockwise;
    }
}

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

int main() {
    Point2d p0 {0.0, 0.0};
    Point2d p1 {1.0, 0.0};
    Point2d p2 {0.0, 1.0};
    Point2d p3 {-1.0, 0.0};
    Point2d p4 {0.5, 0.5};
    Point2d p5 {0.5, -0.5};
    Segment s1 (p0, p1);
    Segment s2 (p0, p2);
    Segment s3 (p0, p3);
    Segment s4 (p4, p5);
    assert(getOrientation(s1, s2) == Orientation::Clockwise);
    assert(getOrientation(s2, s1) == Orientation::Counterclockwise);
    assert(getOrientation(s1, s3) == Orientation::Colinear);
    assert(SegmentsIntersect(s4, s1));
    assert(!SegmentsIntersect(s4, s2));

}