#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <ranges>
#include <set>
#include <tuple>
#include <optional>
#include <utility>
#include <vector>
#include <unordered_map>

namespace sr = std::ranges;

constexpr double tolerance = 1e-6;

bool isClose(double x, double d) {
    return std::fabs(x - d) < tolerance;
}

struct Point2d {
    double x = 0.0;
    double y = 0.0;
    Point2d() = default;
    Point2d(double x, double y) : x {x}, y {y} {}
};

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

bool operator<(const Point2d& lhs, const Point2d& rhs) {
    return rhs.x - lhs.x > tolerance ||
            (isClose(lhs.x, rhs.x) && rhs.y - lhs.y > tolerance);
}

bool operator>=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs < rhs);
}

bool operator>(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x - rhs.x > tolerance ||
           (isClose(lhs.x, rhs.x) && lhs.y - rhs.y > tolerance);
}

bool operator<=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs > rhs);
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
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

Point2d FindIntersection(const Segment& s1, const Segment& s2) {
    const auto& p1 = s1.src;
    const auto& p2 = s1.dst;
    const auto& p3 = s2.src;
    const auto& p4 = s2.dst;
    auto dp1 = p2 - p1;
    auto dp2 = p4 - p3;

    if (isClose(dp1.x, 0.0)) {
        assert(!isClose(dp2.x, 0.0));
        double x_pos = p1.x;
        double y_pos = (dp2.y / dp2.x) * (x_pos - p3.x) + p3.y;
        return Point2d{x_pos, y_pos};
    } else if (isClose(dp2.x, 0.0)) {
        double x_pos = p3.x;
        double y_pos = (dp1.y / dp1.x) * (x_pos - p1.x) + p1.y;
        return Point2d{x_pos, y_pos};
    } else {
        double x_pos = (p3.x * dp2.y / dp2.x - p1.x * dp1.y / dp1.x - p3.y + p1.y) / (dp2.y / dp2.x - dp1.y / dp1.x);
        double y_pos = (dp2.y / dp2.x) * (x_pos - p3.x) + p3.y;
        return Point2d{x_pos, y_pos};
    }
}

std::optional<Point2d> SegmentsIntersect(const Segment& s1, const Segment& s2) {
    const auto& p1 = s1.src;
    const auto& p2 = s1.dst;
    const auto& p3 = s2.src;
    const auto& p4 = s2.dst;
    auto d1 = Direction(p3, p4, p1);
    auto d2 = Direction(p3, p4, p2);
    auto d3 = Direction(p1, p2, p3);
    auto d4 = Direction(p1, p2, p4);
    if (((d1 > tolerance && d2 < -tolerance) || (d1 < -tolerance && d2 > tolerance))
        && ((d3 > tolerance && d4 < -tolerance) || (d3 < -tolerance && d4 > tolerance))) {
        return FindIntersection(s1, s2);
    } else if (isClose(d1, 0.0) && OnSegment(p3, p4, p1)) {
        return p1;
    } else if (isClose(d2, 0.0) && OnSegment(p3, p4, p2)) {
        return p2;
    } else if (isClose(d3, 0.0) && OnSegment(p1, p2, p3)) {
        return p3;
    } else if (isClose(d4, 0.0) && OnSegment(p1, p2, p4)) {
        return p4;
    }
    return {};
}

bool isVertical(const Segment& segment) {
    return isClose(segment.src.x, segment.dst.x);
}

std::set<Point2d> FindAllIntersections(const std::vector<Segment>& S) {
    if (S.size() <= 1) {
        return {};
    }
    // set endpoint by:
    // 1. x-coord < => left-endpoint
    // 2. x.coord == : y-coord < => left-endpoint

    // compare endpoints by:
    // 1. x-coord <
    // 2. if x-coord ==, left-endpoint
    // 3. if x-coord ==, and both are left-endpoints, y-coord <
    // 4. if x.coord ==, and both are left-endpoints, y-coord ==, segment index

    auto endpoints_comparator = [](const auto& e1, const auto& e2) {
        if (e2.first.x - e1.first.x > tolerance) {
            return true;
        } else if (e1.first.x - e2.first.x > tolerance) {
            return false;
        }
        if (std::get<0>(e1.second) < 0) {
            if (std::get<0>(e2.second) >= 0) {
                return true;
            }
        } else if (std::get<0>(e1.second) == 0) {
            if (std::get<0>(e2.second) > 0) {
                return true;
            } else if (std::get<0>(e2.second) < 0) {
                return false;
            }
        } else if (std::get<0>(e2.second) <= 0){
            return false;
        }
        if (e2.first.y - e1.first.y > tolerance) {
            return true;
        } else if (e1.first.y - e2.first.y > tolerance) {
            return false;
        }
        if (std::abs(std::get<0>(e1.second)) < std::abs(std::get<0>(e2.second))) {
            return true;
        } else if (std::abs(std::get<0>(e1.second)) > std::abs(std::get<0>(e2.second))) {
            return false;
        }
        return std::get<1>(e1.second) < std::get<1>(e2.second) ||
                (std::get<1>(e1.second) == std::get<1>(e2.second) && std::get<2>(e1.second) < std::get<2>(e2.second));

    };

    std::set<std::pair<Point2d, std::tuple<int, int, int>>, decltype(endpoints_comparator)> endpoints(endpoints_comparator);

    for (std::size_t i = 0; i < S.size(); i++) {
        auto p1 = S[i].src;
        auto p2 = S[i].dst;
        if (p1.x > p2.x) {
            std::swap(p1, p2);
        }
        if (p1.x == p2.x && p1.y > p2.y) {
            std::swap(p1, p2);
        }
        endpoints.emplace(p1, std::make_tuple(-static_cast<int>(i + 1), 0, 0));
        endpoints.emplace(p2, std::make_tuple(static_cast<int>(i + 1), 0, 0));
    }

    double lm_x = endpoints.begin()->first.x - 10.0;

    auto segment_comparator = [&lm_x](const auto& sp1, const auto& sp2) {
        const auto& s1 = sp1.first;
        const auto& s2 = sp2.first;
        Point2d v1 = s1.dst - s1.src;
        Point2d v2 = s2.dst - s2.src;

        if (v1.x * v2.x > tolerance) {
            auto diff = (v2.y * (lm_x - s2.src.x) + v2.x * s2.src.y) * v1.x - (v1.x * s1.src.y + (lm_x - s1.src.x) * v1.y) * v2.x;
            if (diff > tolerance) {
                return true;
            } else if (diff < -tolerance) {
                return false;
            } else {
                auto cross = Cross(v1, v2);
                if (cross > tolerance) {
                    return true;
                } else if (cross < -tolerance) {
                    return false;
                } else {
                    auto [y1_min, y1_max] = std::minmax(s1.src.y, s1.dst.y);
                    auto [y2_min, y2_max] = std::minmax(s1.src.y, s1.dst.y);
                    if (y2_min < y1_max && y1_min < y2_max) {
                        throw std::runtime_error("Segments overlap");
                    }
                    if (y1_min < y2_min) {
                        return true;
                    } else if (y1_min > y2_min) {
                        return false;
                    } else {
                        auto [x1_min, x1_max] = std::minmax(s1.src.x, s1.dst.x);
                        auto [x2_min, x2_max] = std::minmax(s1.src.x, s1.dst.x);
                        if (x2_min < x1_max && x1_min < x2_max) {
                            throw std::runtime_error("Segments overlap");
                        }
                        return x1_min < x2_min;
                    }
                }
            }
        } else if (v1.x * v2.x < tolerance) {
            auto diff = (v1.x * s1.src.y + (lm_x - s1.src.x) * v1.y) * v2.x - (v2.y * (lm_x - s2.src.x) + v2.x * s2.src.y) * v1.x;
            if (diff > tolerance) {
                return true;
            } else if (diff < -tolerance) {
                return false;
            } else {
                auto cross = Cross(v1, v2);
                if (cross > tolerance) {
                    return true;
                } else if (cross < -tolerance) {
                    return false;
                } else {
                    auto [y1_min, y1_max] = std::minmax(s1.src.y, s1.dst.y);
                    auto [y2_min, y2_max] = std::minmax(s1.src.y, s1.dst.y);
                    if (y2_min < y1_max && y1_min < y2_max) {
                        throw std::runtime_error("Segments overlap");
                    }
                    if (y1_min < y2_min) {
                        return true;
                    } else if (y1_min > y2_min) {
                        return false;
                    } else {
                        auto [x1_min, x1_max] = std::minmax(s1.src.x, s1.dst.x);
                        auto [x2_min, x2_max] = std::minmax(s1.src.x, s1.dst.x);
                        if (x2_min < x1_max && x1_min < x2_max) {
                            throw std::runtime_error("Segments overlap");
                        }
                        return x1_min < x2_min;
                    }
                }
            }
        } else {
            assert(0);
            return false;
        }
    };

    auto vertical_comparator = [](const auto& sp1, const auto& sp2) {
        auto [y1_min, y1_max] = std::minmax(sp1.first.src.y, sp1.first.dst.y);
        auto [y2_min, y2_max] = std::minmax(sp2.first.src.y, sp2.first.dst.y);
        if (y2_min < y1_max && y1_min < y2_max) {
            throw std::runtime_error("Segments overlap");
        }
        return std::min(sp1.first.src.y, sp1.first.dst.y) < std::min(sp2.first.src.y, sp2.first.dst.y);
    };

    using SegmentSet = std::set<std::pair<Segment, int>, decltype(segment_comparator)>;
    using VSegmentSet = std::set<std::pair<Segment, int>, decltype(vertical_comparator)>;
    SegmentSet T (segment_comparator);
    VSegmentSet VT (vertical_comparator);

    std::set<Point2d> intersections;

    std::unordered_map<int, SegmentSet::iterator> iter_map;

    for (const auto& endpoint : endpoints) {
        const auto& p = endpoint.first;
#ifdef DEBUG
        std::cout << "Point " << p << ' ';
#endif
        lm_x = p.x + tolerance * 2.0;
        const auto& [idx, i, j] = endpoint.second;

        if (idx < 0) { // left endpoint
            int index = -idx - 1;
            const auto& segment = S[index];
#ifdef DEBUG
            std::cout << ": left endpoint of " << index << '\n';
#endif
            if (isVertical(segment)) {
                try {
                    VT.emplace(segment, index);
                } catch (const std::runtime_error& e) {
                    throw;
                }
                for (const auto& [other_segment, other_index] : T) {
                    auto p_int = SegmentsIntersect(segment, other_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << other_index << " and " << index << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, other_index, index));
                        intersections.insert(*p_int);
                    }
                }
                for (const auto& [other_segment, other_index] : VT) {
                    if (other_segment == segment) {
                        continue;
                    }
                    auto p_int = SegmentsIntersect(segment, other_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << other_index << " and " << index << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, other_index, index));
                        intersections.insert(*p_int);
                    }
                }
            } else {
                SegmentSet::iterator it;
                try {
                    auto [it_, inserted_] = T.emplace(segment, index);
                    it = it_;
                    iter_map[index] = it;
                } catch (const std::runtime_error& e) {
                    throw;
                }
                if (it != T.begin()) {
                    const auto& [below_segment, below_index] = *std::prev(it);
                    auto p_int = SegmentsIntersect(segment, below_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << below_index << " and " << index << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, below_index, index));
                        intersections.insert(*p_int);
                    }
                }
                if (std::next(it) != T.end()) {
                    const auto& [above_segment, above_index] = *std::next(it);
                    auto p_int = SegmentsIntersect(segment, above_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << index << " and " << above_index << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, index, above_index));
                        intersections.insert(*p_int);
                    }
                }
                for (const auto& [other_segment, other_index] : VT) {
                    auto p_int = SegmentsIntersect(segment, other_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << other_index << " and " << index << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, other_index, index));
                        intersections.insert(*p_int);
                    }
                }
            }
        } else if (idx > 0) { // right endpoint
            int index = idx - 1;
            const auto& segment = S[index];
#ifdef DEBUG
            std::cout << ": right endpoint of " << index << '\n';
#endif
            if (!isVertical(segment)) {
                auto it = iter_map[index];
                if (it != T.begin() && std::next(it) != T.end()) {
                    const auto&[below_segment, below_index] = *std::prev(it);
                    const auto&[above_segment, above_index] = *std::next(it);
                    auto p_int = SegmentsIntersect(above_segment, below_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << below_index << " and " << above_index << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, below_index, above_index));
                        intersections.insert(*p_int);
                    }
                }
#ifdef DEBUG
                std::cout << "Erasing " << it->first << '\n';
#endif
                T.erase(it);
                iter_map.erase(index);
            } else {
                VSegmentSet::iterator it;
                try {
                    it = VT.find({segment, index});
                } catch (const std::runtime_error& e) {
                    throw;
                }
                VT.erase(it);
            }
        } else { // intersection point
            const auto& segment_i = S[i];
            const auto& segment_j = S[j];
#ifdef DEBUG
            std::cout << ": intersection of " << i << ' ' << j << '\n';
#endif
            if (!isVertical(segment_i) && !isVertical(segment_j)) {
                auto it_i = iter_map[i];
                auto it_j = iter_map[j];
                T.erase(it_i);
                T.erase(it_j);
                try {
                    auto [it_i_, inserted_i] = T.emplace(segment_i, i);
                    auto [it_j_, inserted_j] = T.emplace(segment_j, j);
                    it_i = it_i_;
                    it_j = it_j_;
                    iter_map[i] = it_i;
                    iter_map[j] = it_j;
                } catch (const std::runtime_error &e) {
                    throw;
                }
                if (std::next(it_i) != T.end()) {
                    const auto& [above_segment, above_index] = *std::next(it_i);
                    auto p_int = SegmentsIntersect(segment_i, above_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << i << " and " << above_index << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, i, above_index));
                        intersections.insert(*p_int);
                    }
                }
                if (it_j != T.begin()) {
                    const auto& [below_segment, below_index] = *std::prev(it_j);
                    auto p_int = SegmentsIntersect(segment_j, below_segment);
                    if (p_int.has_value()) {
#ifdef DEBUG
                        std::cout << "Inserting " << *p_int << " with " << below_index << " and " << j << '\n';
#endif
                        endpoints.emplace(*p_int, std::make_tuple(0, below_index, j));
                        intersections.insert(*p_int);
                    }
                }
            }
        }
    }

    return intersections;
}

std::set<Point2d> FindAllIntersectionsNaive(const std::vector<Segment>& S) {
    if (S.size() <= 1) {
        return {};
    }
    std::set<Point2d> intersections;

    for (std::size_t i = 0; i < S.size(); i++) {
        for (std::size_t j = i + 1; j < S.size(); j++) {
            auto p_int = SegmentsIntersect(S[i], S[j]);
            if (p_int.has_value()) {
                intersections.insert(*p_int);
            }
        }
    }
    return intersections;
}

int main() {
    constexpr std::size_t seg_size = 10;
    constexpr std::size_t test_cases = 1000;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(-2, 2);

    for (std::size_t c = 0; c < test_cases; c++) {
        std::vector<Segment> segments;

        for (std::size_t i = 0; i < seg_size; i++) {
            Point2d src{static_cast<double>(dist(gen)), static_cast<double>(dist(gen))};
            Point2d dst{static_cast<double>(dist(gen)), static_cast<double>(dist(gen))};
            if (src != dst) {
                segments.emplace_back(src, dst);
            }
        }

#ifdef DEBUG
        std::cout << "Segments:\n";
        for (const auto& segment : segments) {
            std::cout << segment << '\n';
        }
        std::cout << '\n';
#endif

        try {
            auto inter1 = FindAllIntersections(segments);
            auto inter2 = FindAllIntersectionsNaive(segments);
#ifdef DEBUG
            std::cout << inter1.size() << ' ' << inter2.size() << '\n';
            for (const auto& int1 : inter1) {
                std::cout << int1 << '\n';
            }
            std::cout << '\n';
            for (const auto& int2 : inter2) {
                std::cout << int2 << '\n';
            }
            std::cout << '\n';
#endif
            assert(inter1.size() == inter2.size());
        } catch (const std::runtime_error& e) {
#ifdef DEBUG
            std::cerr << e.what() << '\n';
#endif
        }
    }

}