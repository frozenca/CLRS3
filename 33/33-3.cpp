#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <set>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

constexpr double tolerance = 1e-6;

struct Point2d {
    double x = 0.0;
    double y = 0.0;
    Point2d() = default;
    Point2d(double x, double y) : x {x}, y {y} {}
};

bool isClose(double x, double d) {
    return std::fabs(x - d) < tolerance;
}

std::ostream& operator<<(std::ostream& os, const Point2d& point) {
    os << '{' << point.x << ", " << point.y << '}';
    return os;
}

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
    Point2d src;
    Point2d dst;
    Segment(const Point2d& src, const Point2d& dst)
            : src{src}, dst{dst} {
        assert(src != dst);
    }
};

std::ostream& operator<<(std::ostream& os, const Segment& segment) {
    os << '{' << segment.src << ',' << segment.dst << '}';
    return os;
}

bool operator==(const Segment& lhs, const Segment& rhs) {
    if (lhs.src == rhs.src) {
        return lhs.dst == rhs.dst;
    } else if (lhs.src == rhs.dst) {
        return lhs.dst == rhs.src;
    } else {
        return false;
    }
}

bool operator!=(const Segment& lhs, const Segment& rhs) {
    return !(lhs == rhs);
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

enum class Class {
    Ghost,
    GhostBuster,
};

using Actor = std::pair<Point2d, Class>;

void GetRays(std::vector<Actor>& Q, std::vector<Segment>& rays) {
    if (Q.empty()) {
        return;
    }
    auto find_leftlowest = [](const auto& p1, const auto& p2) {
        return p1.first.y < p2.first.y || (isClose(p1.first.y, p2.first.y) && p1.first.x < p2.first.x);
    };
    auto act_it = sr::min_element(Q, find_leftlowest);
    const auto act = *act_it;
    const auto& p = act.first;
    const auto& cls = act.second;
    auto angle_comparator = [&p](const auto& p1, const auto& p2) {
        return Cross(p1.first - p, p2.first - p) > 0.0;
    };
    std::size_t ghs = 0, gbs = 0;
    if (cls == Class::Ghost) {
        ghs++;
    } else if (cls == Class::GhostBuster) {
        gbs++;
    } else {
        assert(0);
    }

    Q.erase(act_it);
    sr::sort(Q, angle_comparator);
    std::vector<Actor> Q_left, Q_right;
    std::size_t i = 0;
    for (; i < Q.size(); i++) {
        const auto& [pt, cl] = Q[i];
        if (cl == Class::Ghost) {
            ghs++;
        } else if (cl == Class::GhostBuster) {
            gbs++;
        } else {
            assert(0);
        }
        if (cl != cls && ghs == gbs) { // match point found
            rays.emplace_back(pt, p);
            break;
        } else {
            Q_right.push_back(Q[i]);
        }
    }
    std::size_t j = i + 1;
    for (; j < Q.size(); j++) {
        Q_left.push_back(Q[j]);
    }
    GetRays(Q_left, rays);
    GetRays(Q_right, rays);
}

std::vector<Segment> GetRays(std::vector<Actor>& Q) {
    std::vector<Segment> rays;
    GetRays(Q, rays);
    return rays;
}

int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(-100.0, 100.0);
    constexpr std::size_t num_points = 30;

    std::vector<Actor> Q;

    for (std::size_t i = 0; i < num_points; i++) {
        Q.emplace_back(Point2d{dist(gen), dist(gen)}, Class::Ghost);
    }
    for (std::size_t i = 0; i < num_points; i++) {
        Q.emplace_back(Point2d{dist(gen), dist(gen)}, Class::GhostBuster);
    }

    std::cout << "Points:\n";
    for (const auto& [p, cls] : Q) {
        std::cout << p << ' ';
        if (cls == Class::Ghost) {
            std::cout << "Ghost" << '\n';
        } else if (cls == Class::GhostBuster) {
            std::cout << "GhostBuster" << '\n';
        } else {
            assert(0);
        }
    }
    std::cout << '\n';

    auto rays = GetRays(Q);
    std::cout << "Rays:\n";
    for (const auto& ray : rays) {
        std::cout << ray << '\n';
    }
    std::cout << '\n';

    assert(!AnySegmentIntersect(rays));
    std::cout << "Rays doesn't intersect each other\n";
}