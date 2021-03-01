#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
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

void SortByPolarAngle(std::vector<Point2d>& P, const Point2d& p0) {
    auto sort_by_angle = [&p0](const auto& p1, const auto& p2) {
        double cross = Cross(p1 - p0, p2 - p0);
        return cross > 0.0 || (isClose(cross, 0.0) && dist(p1, p0) > dist(p2, p0));
    };
    std::set<Point2d, decltype(sort_by_angle)> S_upper(sort_by_angle);
    std::set<Point2d, decltype(sort_by_angle)> S_lower(sort_by_angle);
    for (const auto& p : P) {
        if (p.y > 0.0) {
            S_upper.insert(p);
        } else {
            S_lower.insert(p);
        }
    }
    P.clear();
    for (const auto& p : S_upper) {
        P.push_back(p);
    }
    for (const auto& p : S_lower) {
        P.push_back(p);
    }
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
    for (const auto& [x, y] : Q) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

    SortByPolarAngle(Q, Point2d{0.0, 0.0});

    std::cout << "Points, sorted by polar angle with the origin:\n";
    for (const auto& [x, y] : Q) {
        std::cout << '(' << x << ", " << y << ")\n";
    }
    std::cout << '\n';

}