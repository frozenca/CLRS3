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
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point2d& lhs, const Point2d& rhs) {
    return !(lhs == rhs);
}

double Cross(const Point2d& lhs, const Point2d& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

bool isClose(double x, double d) {
    return std::fabs(x - d) < tolerance;
}

double dist(const Point2d& lhs, const Point2d& rhs) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

double Orientation(const Point2d& p0, const Point2d& p1, const Point2d& p2) {
    return Cross(p2 - p1, p1 - p0);
}

std::vector<Point2d> GrahamScan(const std::vector<Point2d>& Q) {
    assert(Q.size() >= 3);
    auto find_leftlowest = [](const auto& p1, const auto& p2) {
        return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
    };
    const auto& p0 = *sr::min_element(Q, find_leftlowest);
    auto angle_comparator = [&p0](const auto& p1, const auto& p2) {
        return Cross(p1 - p0, p2 - p0) > 0.0;
    };
    std::set<Point2d, decltype(angle_comparator)> P(angle_comparator);
    for (const auto& point : Q) {
        if (point == p0) {
            continue;
        }
        auto it = P.find(point); // does equivalent point exist
        if (it == P.end()) {
            P.insert(point);
        } else {
            auto dist_diff = dist(p0, point) - dist(p0, *it);
            if (dist_diff > 0.0) {
                P.erase(it);
                P.insert(point);
            }
        }
    }
    std::vector<Point2d> S;
    S.push_back(p0);
    S.push_back(*P.begin());
    P.erase(P.begin());
    S.push_back(*P.begin());
    P.erase(P.begin());
    for (const auto& p_i : P) {
        while (Cross(S[S.size() - 1] - S[S.size() - 2], p_i - S[S.size() - 2]) < 0.0) {
            S.pop_back();
        }
        if (isClose(Cross(S[S.size() - 1] - S[S.size() - 2], p_i - S[S.size() - 2]), 0.0)) {
            S.pop_back();
        }
        S.push_back(p_i);
    }
    return S;
}

std::vector<Point2d> ConvexHullIncrementalAdd(std::vector<Point2d>& CH, const Point2d& p) {
    if (CH.size() <= 1) {
        CH.push_back(p);
        return CH;
    } else if (CH.size() == 2) {
        auto cross = Cross(p - CH[0], CH[1] - CH[0]);
        if (cross < -tolerance) { // left turn
            CH.push_back(p);
        } else if (isClose(cross, 0.0)) { // colinear
            CH[1] = p;
        } else { // right turn
            CH.push_back(p);
            std::swap(CH[1], CH[2]);
        }
        return CH;
    } else {
        const std::size_t n = CH.size();
        std::size_t i = 0, j = 0;
        // binary search: find a first/last point p_i that p_i-p_(i+1)-p is a right turn
        if (Orientation(CH[0], CH[1], p) > 0.0) { // p is in lower side
            i = 0;
            std::size_t l = 0, r = n;
            // find first left turn
            while (l < r) {
                auto m = l + (r - l) / 2;
                if (Orientation(CH[m % n], CH[(m + 1) % n], p) > 0.0) {
                    l = m + 1;
                } else {
                    r = m;
                }
            }
            j = l;
        } else if (Orientation(CH[n - 1], CH[0], p) > 0.0) { // p is in upper side
            j = n;
            std::size_t l = 0, r = n;
            // find first right turn
            while (l < r) {
                auto m = l + (r - l) / 2;
                if (Orientation(CH[m % n], CH[(m + 1) % n], p) < 0.0) {
                    l = m + 1;
                } else {
                    r = m;
                }
            }
            i = l;
        } else { // middle side
            std::size_t l = 1, r = n;
            // find first point that the sign of angle with p_0-p-p_i changes
            while (l < r) {
                auto m = l + (r - l) / 2;
                if (Orientation(CH[0], p, CH[m]) > 0.0) {
                    l = m + 1;
                } else {
                    r = m;
                }
            }
            std::size_t k = l;
            l = 0, r = k;
            // find first right turn
            while (l < r) {
                auto m = l + (r - l) / 2;
                if (Orientation(CH[m % n], CH[(m + 1) % n], p) < 0.0) {
                    l = m + 1;
                } else {
                    r = m;
                }
            }
            i = l;
            l = k, r = n - 1;
            // find last right turn
            while (l < r) {
                auto m = l + (r - l) / 2;
                if (Orientation(CH[m % n], CH[(m + 1) % n], p) > 0.0) {
                    l = m + 1;
                } else {
                    r = m;
                }
            }
            j = l;
        }

        // remove (i, j) and replace with p.
        if (i < n - 1 && i < j) {
            CH.erase(CH.begin() + i + 1, CH.begin() + j);
        }
        CH.insert(CH.begin() + i + 1, p);
        return CH;
    }
}

std::vector<Point2d> ConvexHullIncremental(const std::vector<Point2d>& Q_) {
    auto Q = Q_;
    auto point_comparator = [](const auto& p1, const auto& p2) {
        return p1.x < p2.x || (isClose(p1.x, p2.x) && p1.y < p2.y);
    };
    sr::sort(Q, point_comparator);

    std::vector<Point2d> CH;
    for (const auto& p : Q) {
        CH = ConvexHullIncrementalAdd(CH, p);
    }
    return CH;
}

int main() {
    std::vector<Point2d> Q;

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(-100.0, 100.0);
    constexpr std::size_t num_points = 30;
    constexpr std::size_t num_test_cases = 100;

    for (std::size_t t = 0; t < num_test_cases; t++) {
        Q.clear();
        for (std::size_t i = 0; i < num_points; i++) {
            Q.emplace_back(dist(gen), dist(gen));
        }

        auto GS = GrahamScan(Q);
        auto CH = ConvexHullIncremental(Q);
        // verification
        auto it = sr::find(CH, GS[0]);
        assert(it != CH.end());
        sr::rotate(CH, it);
        assert(GS == CH);
    }
    std::cout << "All test cases passed\n";

}