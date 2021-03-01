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

std::vector<Point2d> ConvexHullAdd(std::vector<Point2d>& CH, const Point2d& p) {
    if (CH.empty()) {
        CH.push_back(p);
        return CH;
    } else if (CH.size() == 1) {
        if (CH[0] != p) {
            CH.push_back(p);
        }
        return CH;
    } else if (CH.size() == 2) {
        const auto& p0 = CH[0];
        const auto& p1 = CH[1];
        auto cross = Cross(p0 - p, p1 - p);
        if (isClose(cross, 0.0)) { // colinear
            auto d1 = dist(p0, p);
            auto d2 = dist(p1, p);
            auto d3 = dist(p0, p1);
            auto d = std::max({d1, d2, d3});
            if (isClose(d, d1)) {
                return std::vector<Point2d>{p0, p};
            } else if (isClose(d, d2)) {
                return std::vector<Point2d>{p1, p};
            } else {
                return std::vector<Point2d>{p0, p1};
            }
        }
    }
    const std::size_t m = CH.size();
    std::size_t i = 0;
    // while left turn
    while (i < m && Cross(p - CH[i % m], CH[(i + 1) % m] - CH[i % m]) < -tolerance) {
        i++;
    }
    if (i == m) {
        return CH;
    }
    std::size_t j = (i + 1) % m;
    // while right turn
    while (Cross(p - CH[j % m], CH[(j + 1) % m] - CH[j % m]) > tolerance) {
        j = (j + 1) % m;
    }

    // replace p_(i, j) with p
    if (j < i) {
        CH.erase(CH.begin() + i + 1, CH.end());
        CH.push_back(p);
        CH.erase(CH.begin(), CH.begin() + j);
    } else if (j > i) {
        CH.erase(CH.begin() + i + 1, CH.begin() + j);
        CH.insert(CH.begin() + i + 1, p);
    } else { // cannot happen
        assert(0);
    }
    return CH;
}

std::vector<Point2d> ConvexHullOnline(const std::vector<Point2d>& Q) {
    std::vector<Point2d> CH;
    for (const auto& p : Q) {
        CH = ConvexHullAdd(CH, p);
    }
    return CH;
}

int main() {
    std::vector<Point2d> Q;

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(-100.0, 100.0);
    constexpr std::size_t num_points = 30;

    for (std::size_t i = 0; i < num_points; i++) {
        Q.emplace_back(dist(gen), dist(gen));
    }

    std::cout << "Points:\n";
    for (const auto& p : Q) {
        std::cout << p << '\n';
    }
    std::cout << '\n';

    auto GS = GrahamScan(Q);
    std::cout << "Convex hull by Graham's Scan:\n";
    for (const auto& p : GS) {
        std::cout << p << '\n';
    }
    std::cout << '\n';

    auto CH = ConvexHullOnline(Q);
    std::cout << "Convex hull online:\n";
    for (const auto& p : CH) {
        std::cout << p << '\n';
    }
    std::cout << '\n';
}