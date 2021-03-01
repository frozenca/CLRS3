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

bool isClose(double x, double d) {
    return std::fabs(x - d) < 1e-6;
}

struct Circle {
    Point2d center;
    double radius = 1.0;
    Circle() = default;
    Circle(const Point2d& center, double radius) : center {center}, radius {radius} {
        assert(radius > 0.0);
    }
};

bool AnyCircleIntersect(const std::vector<Circle>& S) {
    if (S.size() <= 1) {
        return false;
    }
    std::vector<std::pair<Point2d, int>> endpoints;
    for (std::size_t i = 0; i < S.size(); i++) {
        endpoints.emplace_back(Point2d{S[i].center.x - S[i].radius, S[i].center.y}, -static_cast<int>(i + 1));
        endpoints.emplace_back(Point2d{S[i].center.x + S[i].radius, S[i].center.y}, static_cast<int>(i + 1));
    }
    auto endpoints_comparator = [](const auto& endpt1, const auto& endpt2) {
        return (endpt1.first.x < endpt2.first.x)
               || (endpt1.first.x == endpt2.first.x && endpt1.second < endpt2.second)
               || (endpt1.first.x == endpt2.first.x && endpt1.second == endpt2.second && endpt1.first.y < endpt1.first.y);
    };
    sr::sort(endpoints, endpoints_comparator);
    std::set<std::pair<Point2d, int>, decltype(endpoints_comparator)> T (endpoints_comparator);
    for (const auto& [p, index] : endpoints) {
        if (index < 0) {
            T.emplace(p, index);
        } else if (index > 0) {
            double r1 = S[index - 1].radius;
            Point2d c1 {p.x + r1, p.y};
            auto it = T.lower_bound({p, index});
            if (it != T.begin()) {
                const auto& [prev_pt, prev_index] = *std::prev(it);
                double r2 = S[-prev_index - 1].radius;
                Point2d c2 {prev_pt.x + r2, prev_pt.y};
                if (std::hypot(c2.x - c1.x, c2.y - c2.y) <= std::hypot(r1, r2)) {
                    return true;
                }
            }
            Point2d left_point {p.x - 2 * r1, p.y};
            T.erase({left_point, -index});
        }
    }
    return false;
}

int main() {
    std::vector<Circle> S {{{0.0, 0.0}, 1.0}, {{2.0, 0.0}, 1.0}};
    assert(AnyCircleIntersect(S));
    std::vector<Circle> S2 {{{0.0, 0.0}, 1.0}, {{3.0, 0.0}, 1.0}};
    assert(!AnyCircleIntersect(S2));
    std::vector<Circle> S3 {{{0.0, 0.0}, 1.0}, {{3.0, 0.0}, 1.0}, {{1.5, 1.5}, 1.5}};
    assert(AnyCircleIntersect(S3));


}