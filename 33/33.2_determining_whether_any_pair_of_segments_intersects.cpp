#include <algorithm>
#include <cassert>
#include <cmath>
#include <ranges>
#include <set>
#include <utility>
#include <vector>

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

struct Segment {
    bool directed = false;
    Point2d src;
    Point2d dst;
    Segment(const Point2d& src, const Point2d& dst, bool directed = false)
        : src{src}, dst{dst}, directed {directed} {
        assert(src != dst);
    }
};

bool isClose(double x, double d) {
    return std::fabs(x - d) < 1e-6;
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

bool AnySegmentIntersect(const std::vector<Segment>& S) {
    if (S.size() <= 1) {
        return false;
    }
    std::vector<std::pair<Point2d, int>> endpoints;
    for (std::size_t i = 0; i < S.size(); i++) {
        endpoints.emplace_back(S[i].src, -static_cast<int>(i + 1));
        endpoints.emplace_back(S[i].dst, static_cast<int>(i + 1));
    }
    auto endpoints_comparator = [](const auto& endpt1, const auto& endpt2) {
        return (endpt1.first.x < endpt2.first.x)
        || (endpt1.first.x == endpt2.first.x && endpt1.second < endpt2.second)
        || (endpt1.first.x == endpt2.first.x && endpt1.second == endpt2.second && endpt1.first.y < endpt1.first.y);
    };
    sr::sort(endpoints, endpoints_comparator);
    auto segment_comparator = [](const auto& segment1, const auto& segment2) {
        return Cross(segment1.dst - segment1.src, segment2.dst - segment2.src) > 0.0;
    };
    std::set<Segment, decltype(segment_comparator)> T (segment_comparator);
    for (const auto& [p, index] : endpoints) {
        if (index < 0) {
            const auto& s = S[-index - 1];
            T.insert(s);
            auto it = T.find(s);
            if (it != T.begin()) {
                const auto& above_s = *std::prev(it);
                if (SegmentsIntersect(above_s, s)) {
                    return true;
                }
            }
            if (it != T.end() && std::next(it) != T.end()) {
                const auto& below_s = *std::next(it);
                if (SegmentsIntersect(below_s, s)) {
                    return true;
                }
            }
        } else if (index > 0) {
            const auto& s = S[index - 1];
            auto it = T.find(s);
            if (it != T.begin() && it != T.end() && std::next(it) != T.end()) {
                const auto& above_s = *std::prev(it);
                const auto& below_s = *std::next(it);
                if (SegmentsIntersect(above_s, below_s)) {
                    return true;
                }
            }
            if (it != T.end()) {
                T.erase(it);
            }
        }
    }
    return false;
}

int main() {
    Segment s1 {Point2d{0.0, 10.0}, Point2d{2.0, 9.0}};
    Segment s2 {Point2d{1.0, 9.0}, Point2d{3.0, 7.0}};
    Segment s3 {Point2d{2.0, 8.0}, Point2d{4.0, 5.0}};
    Segment s4 {Point2d{0.0, 9.0}, Point2d{2.0, 11.0}};
    std::vector<Segment> S1 {s1, s2, s3};
    std::vector<Segment> S2 {s1, s2, s3, s4};
    assert(!AnySegmentIntersect(S1));
    assert(AnySegmentIntersect(S2));


}