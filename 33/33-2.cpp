#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>
#include <ranges>

namespace sr = std::ranges;

struct Point2d {
    double x = 0.0;
    double y = 0.0;
    Point2d() = default;
    Point2d(double x, double y) : x {x}, y {y} {}
};

constexpr double tolerance = 1e-6;

std::ostream& operator<<(std::ostream& os, const Point2d& point) {
    os << '{' << point.x << ", " << point.y << '}';
    return os;
}

bool isClose(double x, double d) {
    return std::fabs(x - d) < tolerance;
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

double dist(const Point2d& lhs, const Point2d& rhs) {
    return std::hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

std::vector<std::vector<Point2d>> MaximalLayers(const std::vector<Point2d>& P_) {
    auto P = P_;
    sr::sort(P, std::greater<>());
    std::vector<Point2d> leftmosts;
    std::vector<std::vector<Point2d>> layers;
    auto y_comp = [](const auto& p1, const auto& p2) {
        return p1.y > p2.y || (isClose(p1.y, p2.y) && p1.x > p2.x);
    };
    for (const auto& p : P) {
        auto j = std::distance(leftmosts.begin(), sr::upper_bound(leftmosts, p, y_comp));
        if (j == layers.size()) {
            leftmosts.push_back(p);
            layers.emplace_back(std::vector<Point2d>{p});
        } else {
            if (leftmosts[j].x == p.x) {
                leftmosts.insert(leftmosts.begin() + j, p);
                layers.insert(layers.begin() + j, std::vector<Point2d>{p});
            } else {
                leftmosts[j] = p;
                layers[j].emplace_back(p);
            }
        }
    }
    return layers;
}

int main() {
    std::vector<Point2d> Q;

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(-10, 10);
    constexpr std::size_t num_points = 30;

    for (std::size_t i = 0; i < num_points; i++) {
        Q.emplace_back(static_cast<double>(dist(gen)), static_cast<double>(dist(gen)));
    }

    std::cout << "Points:\n";
    for (const auto& p : Q) {
        std::cout << p << '\n';
    }
    std::cout << '\n';

    auto ML = MaximalLayers(Q);

    std::cout << "Maximal layers:\n";
    for (const auto& layer : ML) {
        std::cout << "Points in layer:\n";
        for (const auto& p : layer) {
            std::cout << p << '\n';
        }
        std::cout << '\n';
    }

}